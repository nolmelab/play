#pragma once

#include <nlohmann/json.hpp>
#include <play/robust/net/frame_handler.hpp>
#include <play/robust/net/runner/thread_runner.hpp>
#include <play/robust/net/session.hpp>
#include <shared_mutex>
#include <string_view>
#include <unordered_map>

namespace play { namespace robust { namespace net {

// 프로토콜이 지정된 서버로 클라 연결을 받아 통신을 한다.
/**
 * frame_handler를 통해 구독(sub)하고 받는 방식과 subclassing으로 handle_receive 등에서 
 * 처리하는 방법 두 가지를 제공한다. 
 * 
 * @tparam Protocol secure_protocol<uint16_t>와 같은 프로토콜 
 * @tparam Frame flatbuffers::NativeTable과 같은 앱 프레임
 */
template <typename Protocol, typename Frame = frame_subclass>
class server : public session_handler<session<Protocol>>
{
public:
  using session = session<Protocol>;
  using session_ptr = std::shared_ptr<session>;
  using topic = typename Protocol::topic;
  using handle = typename session::handle;
  using frame_handler = frame_handler<session, topic, Frame>;

public:
  // json 문자열에서 읽어 서버를 준비
  server(runner& runner, std::string_view json, frame_handler& handler);

  // frame_subclass_handler를 사용하여 subclass 인터페이스를 지원
  server(runner& runner, std::string_view json);

  // listen()부터 시작하여 서버를 시작
  bool start();

  // 세션 맵에서 세션을 찾아서 돌려줌
  session_ptr get_session(size_t handle);

  template <typename FrameHandler>
  FrameHandler& get_handler() const;

  // 서버를 종료
  void stop();

  // 세션에서 프로토콜 협상 완료 통지
  void on_established(session_ptr se) final;

  // 세션에서 연결 종료 통지
  void on_closed(session_ptr se, error_code ec) final;

  // topic 단위 페이로드를 프로토콜에서 얻은 후 세션을 통해 전달
  void on_receive(session_ptr se, topic topic, const void* data, size_t len) final;

protected:
  virtual bool on_start();

  virtual void on_stop();

  // 하위 클래스에 협상 완료 처리 전달
  virtual void handle_established(session_ptr se) {}

  // 하위 클래스에 연결 종료 처리 전달
  virtual void handle_closed(session_ptr se, error_code ec) {}

  // 하위 클래스에 페이로드 처리 전달
  virtual void handle_receive(session_ptr se, topic topic, const void* data, size_t len) {}

private:
  using session_map = std::unordered_map<handle, session_ptr>;
  using shared_mutex = std::shared_timed_mutex;
  using acceptor = asio::ip::tcp::acceptor;

private:
  // accept를 시작
  void start_accept();

  // accept를 처리. start_accept() 호출
  void handle_accept(session_ptr se, error_code ec);

private:
  runner& runner_;
  std::string json_;
  frame_handler& frame_handler_;
  nlohmann::json jconf_;
  std::unique_ptr<acceptor> acceptor_;
  session_map sessions_;
  shared_mutex mutex_;
};

}}}  // namespace play::robust::net

#include <play/robust/net/server.ipp>