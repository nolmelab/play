#pragma once

#include <play/robust/net/frame_handler.hpp>

namespace play { namespace robust { namespace net {

// 프레임 바이트를 받아서 Frame 포인터를 만들어 전달하는 클래스
/**
 * - subscribe handler with topic 
 * - regiser creator prototype functions for each topic 
 * 
 * server.get_handler().send<app::ack_move>(session, pic, obj);
 */
template <typename Session>
class flatbuffer_handler : public frame_handler<uint16_t, flatbuffers::NativeTable>
{
public:
  using session_ptr = std::shared_ptr<Session>;
  using ptr = typename frame_handler<uint16_t, flatbuffer::NativeTable>::ptr;
  using topic = typename frame_handler<uint16_t, flatbuffer::NativeTable>::topic;

public:
  flatbuffer_handler() = default;

  // flatbuffer_handler를 알고 있는 곳에서만 사용 가능한 함수
  /**
   * send()는 flatbuffers 전용의 전송 편의 함수
   */
  template <typename FlatBufferObj, typename Obj>
  void send(sesion_ptr se, Obj& obj);

  // 등록된 생성기에서 만든 후 구독한 함수(들)에 전달
  /**
   * recv는 handler들의 공통 인터페이스로 반드시 구현이 필요
   */
  void recv(session_ptr se, topic pic, const void* data, size_t len);

  void sub();

  void unsub();

  void reg();

private:
};

}}}  // namespace play::robust::net
