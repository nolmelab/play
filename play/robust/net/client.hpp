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
  /**
   * client는 비동기 연결만 지원
   */
  void connect(std::string_view addr, uint16_t port);

  // 현재 세션을 얻음. 통신 등에 사용
  session_ptr get_session()
  {
    return session_;
  }

  // 연결 종료
  void close();

protected:
  // 하위 클래스에 협상 완료 처리 전달
  virtual void handle_established(session_ptr){};

  // 하위 클래스에 연결 종료 처리 전달
  virtual void handle_closed(session_ptr, boost::system::error_code) {}

  // topic 단위 프레임을 프로토콜에서 얻은 후 session::protocoal_adapter를 통해 전달
  virtual void handle_receive(session_ptr, topic, const void* data, size_t) {};

private:
  void start_connect();

  void reconnect();

  void handle_connect(boost::system::error_code ec);

  // 프로토콜에서 Protocol::adatper를 통해서 전달
  void on_established(session_ptr se) override;

  // 세션에서 연결 종료 통지
  void on_closed(session_ptr se, boost::system::error_code ec) override;

  // topic 단위 프레임을 프로토콜에서 얻은 후 session::protocoal_adapter를 통해 전달
  void on_receive(session_ptr se, topic topic, const void* data, size_t len) override;

private:
  runner& runner_;
  std::string addr_;
  uint16_t port_;
  tcp::endpoint endpoint_;
  session_ptr session_;
};

}}}  // namespace play::robust::net

#include <play/robust/net/client.ipp>