#pragma once

#include <nlohmann/json.hpp>
#include <play/robust/net/runner/thread_runner.hpp>
#include <play/robust/net/session.hpp>
#include <play/robust/net/session_handler.hpp>
#include <shared_mutex>
#include <string_view>
#include <unordered_map>

namespace play { namespace robust { namespace net {

// 프로토콜이 지정된 클라이언트로 서버와 연결하여 통신한다.
/**
 * 연결이 실패하면 재연결을 시도한다. 연결이 끊어지면 설정에 따라 재연결
 */
template <typename Protocol>
class client : public session_handler<Protocol>
{
public:
  using session = session<Protocol>;
  using handle = typename session::handle;
  using session_ptr = typename session_handler<Protocol>::session_ptr;
  using topic = typename Protocol::topic;

public:
  client(runner& runner);

  // 연결을 시작. ip:port 형식 주소.
  void connect(std::string_view addr);

  // 연결 종료
  void close();

private:
  void start_connect();

  void reconnect();

  void handle_connect(boost::system::error_code ec);

  // 프로토콜에서 Protocol::adatper를 통해서 전달
  void on_established(session_ptr session) override;

  void on_closed(session_ptr session, boost::system::error_code ec) override;

  void on_receive(session_ptr session, topic topic, const void* data, size_t len) override;

private:
  runner& runner_;
  std::string addr_;
  tcp::endpoint endpoint_;
  session_ptr session_;
  std::unique_ptr<tcp::socket> socket_;
};

}}}  // namespace play::robust::net

#include <play/robust/net/client.ipp>