#pragma once

#include <nlohmann/json.hpp>
#include <play/robust/net/runner/thread_runner.hpp>
#include <play/robust/net/session.hpp>
#include <play/robust/net/session_handler.hpp>
#include <shared_mutex>
#include <string_view>
#include <unordered_map>

namespace play { namespace robust { namespace net {

// 프로토콜이 지정된 서버로 클라 연결을 받아 통신을 한다.
/**
 *  
 */
template <typename Protocol>
class server : public session_handler<Protocol>
{
public:
  using session = session<Protocol>;
  using handle = typename session::handle;
  using session_ptr = typename session_handler<Protocol>::session_ptr;

public:
  // json 문자열에서 읽어 서버를 준비
  server(std::string_view json);

  // listen()부터 시작하여 서버를 시작
  bool start();

  // 서버를 종료
  void stop();

protected:
  virtual bool on_start();

  virtual void on_stop();

private:
  using session_map = std::unordered_map<handle, session_ptr>;
  using shared_mutex = std::shared_timed_mutex;
  using acceptor = asio::ip::tcp::acceptor;

private:
  // accept를 시작
  void start_accept();

  // accept를 처리. start_accept() 호출
  void handle_accept(boost::system::error_code ec, tcp::socket&& socket);

  // 프로토콜에서 Protocol::adatper를 통해서 전달
  void on_established(session_ptr session) override;

  void on_closed(session_ptr session, boost::system::error_code ec) override;

  void on_receive(session_ptr session, typename Protocol::topic topic, const void* data,
                  size_t len) override;

private:
  std::string json_;
  nlohmann::json jconf_;
  std::unique_ptr<thread_runner> runner_;
  std::unique_ptr<acceptor> acceptor_;
  session_map sessions_;
  shared_mutex mutex_;
};

}}}  // namespace play::robust::net

#include <play/robust/net/server.ipp>