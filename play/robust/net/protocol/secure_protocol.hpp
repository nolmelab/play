#pragma once

#include <play/robust/net/protocol.hpp>
#include <play/robust/net/protocol/length_delimited.hpp>
#include <play/robust/net/protocol/sodium_cipher.hpp>
#include <play/robust/net/protocol/sodium_handshake.hpp>

namespace play { namespace robust { namespace net {

// length delimited and sodium_cipher optional topic field protocol
/**
 * session이 바이트 스트림의 송수신을 담당하고 protocol은 형식에 따른 처리를 하여 
 * 프레임을 전송하거나 프레임을 얻어 전달한다. 
 * 
 * 프레임의 형식은 아래와 같다
 * - topic{sizeof(Topic)} | length{4} | enc{1} | payload
 */
template <typename Topic>
class secure_protocol : public protocol<Topic>
{
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
   * 암호화 협상을 키를 생성하고 전송하여 시작
   * receive()로 세션에서 전달된 값들로 완료하고 established가 되면 adapter로 알림
   */
  void accepted();

  // listen()과 동일하게 진행. 클라 모드.
  void connected();

  // 세션 종료를 받음
  void closed();

  // topic과 길이를 추가하여 send_fn을 통해 전송
  /**
   * encrypt에 따라 암호화 여부를 결정. topic 바로 뒤에 한 바이트로 기록
   * 길이 추가하고 암호화 필요시 암호화한 후에 adapter::send_로 전송
   */
  size_t encode(Topic topic, const char* data, size_t len, codec::mutable_buffer& dest,
                bool encrypt = false);

  // 읽기 변환을 프로토콜에 맞게 하여 프레임에 맞게 돌려준다.
  /**
   * 세션에서는 프로토콜 수신하면 이 함수를 호출. 프로토콜이 협상 데이터로 사용 여부 결정
   */
  std::pair<size_t, std::optional<codec::const_buffer>> decode(const char* data, size_t len);

  bool is_established() const
  {
    return cipher_handshake_->is_established();
  }

private:
  // 세션에서 받은 바이트를 전달. 프로토콜 협상 처리. 최종적으로 recv_fn을 통해 전달
  /**
   * sodium_handshake를 통해 협상 진행. 완료되면 adapter::established_를 통해 알림 
   * 협상 완료 후에는 adatper::forward_ 함수를 통해 프레임을 만들어 전달
   */
  void recv(const char* data, size_t len);

  size_t get_min_size() const
  {
    return length_codec_->length_field_size + sizeof(Topic);
  }

private:
  sodium_handshake::send_fn send_fn_;
  size_t handle_;
  bool accepted_;
  bool connected_;
  bool closed_;

  asio::stream_buf recv_buf_;  // 협상용 통신 처리에만 사용

  std::unique_ptr<length_delimited> length_codec_;
  std::unique_ptr<sodium_cipher> cipher_codec_;
  std::unique_ptr<sodium_handshake> cipher_handshake_;
};

}}}  // namespace play::robust::net

// template member function implementation
#include <play/robust/net/protocol/secure_protocol.ipp>