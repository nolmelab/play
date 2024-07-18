#pragma once

#include <play/robust/net/protocol.hpp>
#include <play/robust/net/protocol/length_delimited.hpp>
#include <play/robust/net/protocol/sodium_cipher.hpp>
#include <play/robust/net/protocol/sodium_handshake.hpp>

namespace play { namespace robust { namespace net {

// Topic으로 구분하고 길이를 포함하는 암호화 가능한 프로토콜
/**
 * session이 바이트 스트림의 송수신을 담당하고 protocol은 형식에 따른 처리를 하여 
 * 프레임을 전송하거나 프레임을 얻어 전달한다. 
 * 
 * 프레임의 형식은 아래와 같다
 * - topic{sizeof(Topic)} | enc{1} | length{4} | payload
 */
template <typename Topic>
class secure_protocol : public protocol<Topic>
{
  static constexpr size_t length_field_size = 4;
  static constexpr size_t enc_field_size = 1;

public:
  secure_protocol(size_t handle)
      : handle_{handle},
        accepted_{false},
        connected_{false},
        closed_{true}
  {
  }

  // listen()에서 받은 세션에서 통지
  /**
   * 암호화 협상을 키를 생성하고 돌려줌. 그 값을 세션에서 전송
   */
  asio::const_buffer accepted();

  // listen()과 동일하게 진행. 클라 모드.
  asio::const_buffer connected();

  // 세션 종료를 받음
  void closed();

  // 프레임으로 만들어 dst에 저장함
  /**
   * encrypt에 따라 암호화 여부를 결정. topic 바로 뒤에 한 바이트로 기록
   * 길이 추가하고 암호화 필요시 암호화한 후 dst에 씀
   * 
   * @param pic topic
   * @param src data buffer to send
   * @param dst session accumulation buffer
   * @return total length
   */
  size_t encode(Topic topic, const asio::const_buffer& src, asio::streambuf& dst,
                bool encrypt = false);

  // 읽기 변환을 프로토콜에 맞게 하여 프레임에 맞게 돌려준다.
  std::tuple<size_t, asio::const_buffer, Topic> decode(const asio::const_buffer& src);

  // 세션에서 받은 바이트를 전달. (established 이전). 결과를 세션에서 전송
  /**
   * sodium_handshake를 통해 협상 진행
   * 
   * @param data 세션에서 받은 데이터를 전달
   * @return <사용한 바이트 수, 전송할 데이터>를 돌려줌. 세션에서 데이터가 있으면 전송. 
   *         사용한 만큼 consume()
   */
  std::pair<size_t, asio::const_buffer> handshake(const asio::const_buffer& src);

  bool is_established() const
  {
    return cipher_handshake_ && cipher_handshake_->is_established();
  }

private:
  size_t get_min_size() const
  {
    return length_field_size + sizeof(Topic) + enc_field_size;
  }

  asio::const_buffer encode_handshake(const asio::const_buffer& src);

private:
  size_t handle_;
  bool accepted_;
  bool connected_;
  bool closed_;

  std::unique_ptr<sodium_cipher> cipher_codec_;
  std::unique_ptr<sodium_handshake> cipher_handshake_;

  length_delimited hs_length_codec_{};
  asio::streambuf hs_send_buf_;  // 협상 전송용 버퍼
};

}}}  // namespace play::robust::net

// template member function implementation
#include <play/robust/net/protocol/secure_protocol.ipp>