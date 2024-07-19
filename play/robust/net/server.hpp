#pragma once

#include <nlohmann/json.hpp>
#include <play/robust/net/runner/thread_runner.hpp>
#include <play/robust/net/session.hpp>
#include <shared_mutex>
#include <string_view>
#include <unordered_map>

namespace play { namespace robust { namespace net {

// 프로토콜이 지정된 서버로 클라 연결을 받아 통신을 한다.
/**
 *  
 */
template <typename Protocol>
class server
{
public:
  using session = session<Protocol, server<Protocol>>;
  using session_ptr = std::shared_ptr<session>;
  using handle = typename session::handle;
  using topic = typename Protocol::topic;

public:
  // json 문자열에서 읽어 서버를 준비
  server(runner& runner, std::string_view json);

  // listen()부터 시작하여 서버를 시작
  bool start();

  // 세션 맵에서 세션을 찾아서 돌려줌
  session_ptr get_session(size_t handle);

  // 서버를 종료
  void stop();

  // 세션에서 프로토콜 협상 완료 통지
  void on_established(session_ptr se);

  // 세션에서 연결 종료 통지
  void on_closed(session_ptr se, error_code ec);

  // topic 단위 페이로드를 프로토콜에서 얻은 후 세션을 통해 전달
  void on_receive(session_ptr se, topic topic, const void* data, size_t len);

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
  nlohmann::json jconf_;
  std::unique_ptr<acceptor> acceptor_;
  session_map sessions_;
  shared_mutex mutex_;
};

}}}  // namespace play::robust::net

#include <play/robust/net/server.ipp>