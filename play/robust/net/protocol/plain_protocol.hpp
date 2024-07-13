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
  using adapter = typename protocol<Topic>::adapter;

public:
  plain_protocol(size_t handle, adapter adapter)
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
  void accepted();

  // listen()과 동일하게 진행. 클라 모드.
  void connected();

  // 세션 종료를 받음
  void closed();

  // topic과 길이를 추가하여 send_fn을 통해 전송
  void send(Topic topic, const char* data, size_t len, bool encrypt = false);

  // 세션에서 받은 바이트를 전달. 프로토콜 협상 처리. 최종적으로 recv_fn을 통해 전달
  void receive(const char* data, size_t len);

  bool is_established() const { return (accepted_ || connected_) && !closed_; }

private:
  adapter& get_adapter() const { return protocol<Topic>::get_adapter(); }

  size_t get_min_size() const { return length_codec_->length_field_size + sizeof(Topic); }

private:
  size_t handle_;
  bool accepted_;
  bool connected_;
  bool closed_;

  asio::streambuf recv_buf_;
  asio::streambuf send_buf_;
  asio::streambuf enc_buf_;

  std::unique_ptr<length_delimited> length_codec_;
};

}}}  // namespace play::robust::net

// template member function implementation
#include <play/robust/net/protocol/plain_protocol.ipp>