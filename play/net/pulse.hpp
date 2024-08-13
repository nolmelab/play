#pragma once

#include <play/net/client.hpp>
#include <play/net/pulse_listener.hpp>
#include <play/net/server.hpp>
#include <play/net/session.hpp>

namespace play {

// 통신 처리의 박동, pulse.
/**
 * Frame을 알고 서버와 클라를 갖고 구독을 처리
 * - server 내장, client 내장, 자식 펄스, 독립적인 펄스의 모드를 가짐
 * - start()할 때 각 모드에 맞게 시작.
 * - with_strand()로 키를 지정하면 해당 키로 strand를 선택하여 처리 
 * - with_session()으로 세션을 지정하면 구독을 세션으로도 필터링
 * - post(), once(), repeat()로 이어처리, 타이머 처리를 함.
 * 
 * - TODO: 원격지 전송 call()과 단선시 에러 콜백
 */
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
  using call_receiver = std::function<void()>;

  using server = server<Protocol>;
  using client = client<Protocol>;

  inline static constexpr topic topic_estalished = topic{1};
  inline static constexpr topic topic_closed = topic{2};

public:
  pulse() = default;

  ~pulse();

  // 서버를 내장한 모드 설정
  pulse& as_server(runner* runner, uint16_t port);

  // 클라이언트를 내장한 모드 설정
  pulse& as_client(runner* runner, const std::string& addr);

  // 독립적인 펄스로 설정. 펄스 테스트용
  pulse& as_independent(runner* runner);

  // 다른 펄스의 자식으로 설정
  pulse& as_child(pulse* parent);

  // 세션 필터링 지정
  /**
   * subscribe 호출 전에 지정하면 해당 세션에 대해서만 관심을 등록. 
   * 모든 세션에 대해 구독하는 경우 with_session() 호출 전에 구독해야 함.
   * start() 이후에 세션 연결이나 변경이 가능. 이전 세션 구독은 동작하지 않으므로 주의 필요
   */
  pulse& with_session(session_ptr ss);

  // 세션이 지정된 가장 가까운 부모의 세션을 받아서 세션으로 지정. with_session과 동일 효과
  pulse& inherit_session();

  // asio strand 키 지정
  pulse& with_strand(size_t key);

  // start pulse
  bool start();

  // stop pulse
  void stop();

  // topic 구독
  template <typename TopicInput>
  pulse& subscribe(TopicInput topic, receiver cb);

  template <typename TopicInput>
  pulse& sub(TopicInput topic, receiver cb)
  {
    return subscribe(topic, cb);
  }

  // 특정 토픽의 구독 여부 확인
  template <typename TopicInput>
  bool has_subscription(TopicInput topic) const;

  // 구독 테스트용.
  void publish(session_ptr se, topic pic, frame_ptr frame);

  // runner를 통한 post(). strand가 있으면 strand로 post()
  template <typename CompletionToken>
  void post(CompletionToken&& handler);

  // runner를 통한 once(). strand가 있으면 strand로 post()
  template <typename CompletionToken>
  timer::ref once(asio::chrono::milliseconds ms, CompletionToken&& handler);

  // runner를 통한 repeat(). strand가 있으면 strand로 post()
  template <typename CompletionToken>
  timer::ref repeat(asio::chrono::milliseconds ms, CompletionToken&& handler);

  // pulse_listener::on_receive
  void on_receive(session_ptr se, topic pic, const void* data, size_t len) final;

  // pulse_listener::on_established
  void on_established(session_ptr se) final;

  // pulse_listener::on_closed
  void on_closed(session_ptr se, error_code ec) final;

  // mode::client일 경우 연결이 끊어지면 재연결 요청이 가능
  bool connect(std::string_view addr, uint16_t port);

protected:
  // 프레임을 구체적인 하위 클래스에서 생성
  virtual frame_ptr unpack(topic pic, const uint8_t* data, size_t len) = 0;

  // 하위 클래스에서 call 구현에 활용
  /**
   * call()은 서버 간 통신에서 단선을 일관되게 처리하기위한 기능. 
   * call()로 요청한 내용은 기억하고 단선시 call_receiver로 호출. 
   * with_session()으로 세션을 지정한 경우 세션 단위로만 동작.
   */
  template <typename TopicInput>
  void call(TopicInput request, TopicInput response, call_receiver cb);

  session_ptr get_session();

private:
  enum class mode : uint8_t
  {
    none,
    server,
    client,
    child,
    independent
  };

  struct subscription
  {
    uintptr_t session_key;
    receiver cb;
  };

  struct caller
  {
    size_t call_id{0};
    call_receiver cb;
  };

  struct topic_calls
  {
    size_t call_index{0};
    size_t reply_index{0};
    std::deque<caller> callers;  // 큐로 동작
  };

  using subscriber_map = std::map<topic, std::vector<subscription>>;
  using child_map = std::map<uintptr_t, pulse*>;
  using shared_mutex = std::shared_timed_mutex;
  using interest_key = std::pair<uintptr_t, topic>;
  using interest_map = std::map<interest_key, child_map>;
  using interest_keep_map = std::map<interest_key, bool>;
  using call_map = std::map<topic, topic_calls>;
  using call_pair_map = std::map<topic, topic>;  // res / req mapping

private:
  // 자식 펄스를 연결
  void bind_child(pulse* child);

  // 자식 펄스 연결 해제
  void unbind_child(pulse* child);

  // 루트에 관심을 알림
  void show_interest(pulse* child, uintptr_t skey, topic pic);

  // 구독한 함수들에 전달. 스트랜드 여부 반영. 자식들 dispatch() 호출
  /**
   * 핸들러 코드에서 구독이나 call 실행 중 다시 구독이나 call 하는 경우가 많아 
   * post()로 다음 실행 시 처리함.  
   */
  void dispatch(session_ptr se, topic pic, frame_ptr frame, bool from_root = false);

  // 특정 call에 대한 응답 구독 여부
  void call_subscribe_reply(topic pic);

  void call_subscribe_closed();

  void call_closed(session_ptr se);

  void call_receive(session_ptr se, topic pic);

  // 루트에 등록한 관심 그룹 처리를 분리하여 배포 여부를 결정
  bool is_target(uintptr_t sub_key, uintptr_t key, bool from_root) const;

  // 나로 부터 시작하여 부모에서 runner를 얻음
  runner* get_runner();

  // 부모가 없는지 확인
  bool is_root() const;

  // 루트 부모를 얻음
  pulse* get_root();

private:
  mode mode_{mode::none};
  mutable shared_mutex sub_mutex_;
  subscriber_map subscriptions_;
  interest_map interests_;
  interest_keep_map self_interests_;
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

  mutable shared_mutex call_mutex_;
  call_map calls_;
  call_pair_map call_pairs_;
};

}  // namespace play

#include <play/net/pulse.ipp>