#pragma once

#include <nlohmann/json.hpp>
#include <play/base/observer_ptr.hpp>
#include <play/net/pulse_listener.hpp>
#include <play/net/runner/thread_runner.hpp>
#include <play/net/session.hpp>
#include <shared_mutex>
#include <string_view>
#include <unordered_map>

namespace play {

// 프로토콜이 지정된 서버로 클라 연결을 받아 통신을 한다.
/**
 * @tparam Protocol secure_protocol<uint16_t>와 같은 프로토콜 
 */
template <typename Protocol>
class server : public session_handler<session<Protocol>>
{
public:
  using session = session<Protocol>;
  using session_ptr = std::shared_ptr<session>;
  using topic = typename Protocol::topic;
  using handle = typename session::handle;
  using session_handler = session_handler<session>;
  using pulse_listener = pulse_listener<session>;

public:
  server(runner& runner);

  bool start(uint16_t port);

  // 세션 맵에서 세션을 찾아서 돌려줌
  session_ptr get_session(size_t handle);

  // 서버를 종료
  void stop();

  // 펄스 연결
  void bind_pulse(pulse_listener* listener);

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
  std::unique_ptr<acceptor> acceptor_;
  pulse_listener* pulse_{nullptr};
  shared_mutex mutex_;
  session_map sessions_;
};

}  // namespace play

#include <play/net/server.ipp>