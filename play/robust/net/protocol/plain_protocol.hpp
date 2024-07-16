#pragma once

#include <play/robust/net/protocol.hpp>
#include <play/robust/net/protocol/length_delimited.hpp>

namespace play { namespace robust { namespace net {

// length delimited and topic field protocol
/**
 * session이 바이트 스트림의 송수신을 담당하고 protocol은 형식에 따른 처리를 하여 
 * 프레임을 전송하거나 프레임을 얻어 전달한다. 
 * 
 * 프레임의 형식은 아래와 같다
 * - topic{sizeof(Topic)} | length{4} | payload
 */
template <typename Topic>
class plain_protocol : public protocol<Topic>
{
public:
  using topic = Topic;
  using adapter = typename protocol<Topic>::adapter;

public:
  plain_protocol(size_t handle, adapter& adapter)
      : protocol<Topic>(adapter),
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
  asio::const_buffer accepted();

  // listen()과 동일하게 진행. 클라 모드.
  asio::const_bufer connected();

  // 세션 종료를 받음
  void closed();

  // 토픽을 포함하여 인코딩.
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
    return length_codec_->length_field_size + sizeof(Topic);
  }

private:
  size_t handle_;
  bool accepted_;
  bool connected_;
  bool closed_;

  std::unique_ptr<length_delimited> length_codec_;
};

}}}  // namespace play::robust::net

// template member function implementation
#include <play/robust/net/protocol/plain_protocol.ipp>