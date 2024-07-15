#pragma once

#include <play/robust/net/protocol.hpp>
#include <play/robust/net/protocol/length_delimited.hpp>

namespace play { namespace robust { namespace net {

// stream_protocol 전용 토픽.
struct stream_protocol_topic
{
};

// 바이트 스트림을 그대로 전달하는 프로토콜. bypass 프로토콜.
class stream_protocol : public protocol<stream_protocol_topic>
{
public:
  using topic = stream_protocol_topic;
  using adapter = typename protocol<topic>::adapter;

public:
  stream_protocol(size_t handle, adapter& adapter)
      : protocol<topic>(adapter),
        handle_{handle},
        accepted_{false},
        connected_{false},
        closed_{true}
  {
  }

  // listen()에서 받은 세션에서 통지
  /**
   * 별도 협상이 필요 없어 바로 established 상태가 됨
   */
  void accepted();

  // listen()과 동일하게 진행. 클라 모드.
  void connected();

  // 세션 종료를 받음
  void closed();

  // send_fn을 통해 전송
  void send(const char* data, size_t len);

  // 세션에서 받은 바이트를 전달.
  void receive(const char* data, size_t len);

  bool is_established() const
  {
    return (accepted_ || connected_) && !closed_;
  }

private:
  adapter& get_adapter()
  {
    return protocol<topic>::get_adapter();
  }

  size_t get_min_size() const
  {
    return 0;
  }

private:
  size_t handle_;
  bool accepted_;
  bool connected_;
  bool closed_;
};

}}}  // namespace play::robust::net

// template member function implementation
#include <play/robust/net/protocol/stream_protocol.ipp>