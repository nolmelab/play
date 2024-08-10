#pragma once

#include <nlohmann/json.hpp>
#include <play/base/observer_ptr.hpp>
#include <play/net/pulse_listener.hpp>
#include <play/net/runner/thread_runner.hpp>
#include <play/net/session.hpp>
#include <play/net/session_handler.hpp>
#include <shared_mutex>
#include <string_view>
#include <unordered_map>

namespace play {

// 프로토콜이 지정된 클라이언트로 서버와 연결하여 통신한다.
/**
 * 연결이 실패하면 재연결을 시도한다. 연결이 끊어지면 통지만 한다.
 * 
 * @tparam Protocol secure_protocol<uint16_t>와 같은 프로토콜 
 */
template <typename Protocol>
class client : public session_handler<session<Protocol>>
{
public:
  using session = session<Protocol>;
  using handle = typename session::handle;
  using session_ptr = std::shared_ptr<session>;
  using topic = typename Protocol::topic;
  using pulse_listener = pulse_listener<session>;

public:
  client(runner& runner);

  // 연결을 시작. ip:port 형식 주소.
  /**
   * client는 비동기 연결만 지원
   * 
   * @return false면 이미 연결된 상태. 로그 확인.
   */
  bool connect(std::string_view addr, uint16_t port);

  // 현재 세션을 얻음. 통신 등에 사용
  session_ptr get_session()
  {
    return session_;
  }

  // 연결 종료
  void close();

  void bind_pulse(pulse_listener* listener);

  // 세션에서 프로토콜 협상 완료 통지
  void on_established(session_ptr se) final;

  // 세션에서 연결 종료 통지
  void on_closed(session_ptr se, error_code ec) final;

  // topic 단위 페이로드를 프로토콜에서 얻은 후 세션을 통해 전달
  void on_receive(session_ptr se, topic topic, const void* data, size_t len) final;

protected:
  // 하위 클래스에 협상 완료 처리 전달
  virtual void handle_established(session_ptr){};

  // 하위 클래스에 연결 종료 처리 전달
  virtual void handle_closed(session_ptr, error_code) {}

  // 하위 클래스에 페이로드 처리 전달
  virtual void handle_receive(session_ptr, topic, const void* data, size_t) {};

private:
  void reconnect();

  void handle_connect(error_code ec);

private:
  runner& runner_;
  std::string addr_;
  uint16_t port_;
  tcp::endpoint endpoint_;
  session_ptr session_;
  pulse_listener* pulse_{nullptr};
};

}  // namespace play

#include <play/net/client.ipp>