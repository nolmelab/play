#pragma once

#include <optional>
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

public:
  stream_protocol(size_t handle)
      : handle_{handle},
        accepted_{false},
        connected_{false},
        closed_{true}
  {
  }

  // listen()에서 받은 세션에서 통지
  /**
   * 바로 협상 완료되므로 value 없이 전달
   * @return 협상에 필요한 정보 전달. 없으면 established 상태
   */
  asio::const_buffer accepted();

  // listen()과 동일하게 진행. 클라 모드.
  asio::const_buffer connected();

  // 세션 종료를 받음
  void closed();

  // 토픽을 포함하여 인코딩. duck typing에 맞추기 위함
  /**
   * dest 버퍼를 prepare 하고, encode를 하여 dest 버퍼에 쓰고, commit 한다.
   * @param pic topic
   * @param data data to send
   * @param len length of the data
   * @param dest session accumulation buffer
   * @return <total length, send payload buffer>
   */
  size_t encode(topic pic, const char* data, size_t len, asio::streambuf& dest);

  // 세션에서 받은 바이트를 전달. (established 이후)
  std::tuple<size_t, asio::const_buffer, topic> decode(const char* data, size_t len);

  // 세션에서 받은 바이트를 전달. (established 이전). 결과를 세션에서 전송
  std::pair<size_t, asio::const_buffer> handshake(const char* data, size_t len);

  bool is_established() const
  {
    return (accepted_ || connected_) && !closed_;
  }

private:
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