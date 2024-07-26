#pragma once

namespace play {

// 세션에서 통지를 위해 사용하는 인터페이스
/**
 * client와 server가 이 인터페이스를 구현하고 하위 클래스로 이어가거나 
 * frame_handler 기반으로 통지한다.
 */
template <typename Session>
struct session_handler
{
  using session_ptr = std::shared_ptr<Session>;
  using topic = typename Session::topic;

  // 세션에서 프로토콜 협상 완료 통지
  virtual void on_established(session_ptr se) = 0;

  // 세션에서 연결 종료 통지
  virtual void on_closed(session_ptr se, error_code ec) = 0;

  // topic 단위 페이로드를 프로토콜에서 얻은 후 세션을 통해 전달
  virtual void on_receive(session_ptr se, topic topic, const void* data, size_t len) = 0;
};

}  // namespace play