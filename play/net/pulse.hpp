#pragma once

#include <play/net/client.hpp>
#include <play/net/pulse_listener.hpp>
#include <play/net/server.hpp>
#include <play/net/session.hpp>

namespace play {

template <typename Protocol, typename Frame>
class pulse : public pulse_listener<session<Protocol>>
{
public:
  using protocol = Protocol;
  using session = session<Protocol>;
  using frame = Frame;
  using session_ptr = std::shared_ptr<session>;
  using frame_ptr = std::shared_ptr<frame>;
  using topic = typename Protocol::topic;
  using receiver = std::function<void(session_ptr, frame_ptr)>;

  using server = server<Protocol>;
  using client = client<Protocol>;

  inline static constexpr topic topic_estalished = topic{1};
  inline static constexpr topic topic_closed = topic{2};

public:
  pulse() = default;

  ~pulse();

  // 서버를 내장한 모드 설정
  pulse& as_server(uint16_t port);

  // 클라이언트를 내장한 모드 설정
  pulse& as_client(const std::string& addr);

  // 다른 펄스의 자식으로 설정
  pulse& as_child(pulse* parent);

  // 독립적인 펄스로 설정
  pulse& as_independent();

  // runner 지정
  pulse& with_runner(runner* runner);

  // 세션 필터링 지정
  pulse& with_session(session_ptr ss);

  // asio strand 키 지정
  pulse& with_strand(size_t key);

  // start pulse
  bool start();

  // stop pulse
  void stop();

  // topic 구독
  template <typename TopicInput>
  pulse& subscribe(TopicInput topic, receiver cb);

  // runner를 통한 post(). strand가 있으면 strand로 post()
  template <typename CompletionToken>
  void post(CompletionToken&& handler);

  // runner를 통한 once(). strand가 있으면 strand로 post()
  template <typename CompletionToken>
  timer::ref once(asio::chrono::milliseconds ms, CompletionToken&& handler);

  // runner를 통한 repeat(). strand가 있으면 strand로 post()
  template <typename CompletionToken>
  timer::ref repeat(asio::chrono::milliseconds ms, CompletionToken&& handler);

  // TODO:
  // call(), reply()

  // pulse_listener::on_receive
  void on_receive(session_ptr se, topic pic, const void* data, size_t len) final;

  // pulse_listener::on_established
  void on_established(session_ptr se) final;

  // pulse_listener::on_closed
  void on_closed(session_ptr se, error_code ec) final;

protected:
  // 프레임을 구체적인 하위 클래스에서 생성
  virtual frame_ptr unpack(topic pic, const uint8_t* data, size_t len) = 0;

private:
  enum class mode : uint8_t
  {
    server,
    client,
    child,
    independent
  };

  struct subscription
  {
    receiver cb;
  };

  using subscriber_map = std::map<topic, std::vector<subscription>>;
  using child_map = std::map<uintptr_t, pulse*>;
  using shared_mutex = std::shared_timed_mutex;

private:
  // 자식 펄스를 연결
  void bind_child(pulse* child);

  // 자식 펄스 연결 해제
  void unbind_child(pulse* child);

  // 구독한 함수들에 전달. 스트랜드 여부 반영. 자식들 dispatch() 호출
  void dispatch(session_ptr se, topic pic, frame_ptr frame);

  // 나로 부터 시작하여 부모에서 runner를 얻음
  runner* get_runner();

private:
  mode mode_;
  mutable shared_mutex mutex_;
  subscriber_map subscriptions_;
  runner* runner_{nullptr};
  bool stop_{false};

  std::unique_ptr<server> server_;
  std::unique_ptr<client> client_;
  pulse* parent_{nullptr};
  child_map childs_;

  uint16_t port_{0};
  std::string addr_;
  session_ptr session_;
  size_t strand_key_{0};
};

}  // namespace play

#include <play/net/pulse.ipp>