#pragma once

#include <optional>
#include <play/robust/net/asio.hpp>
#include <play/robust/net/codec.hpp>
#include <play/robust/net/protocol/sodium_handshake.hpp>

namespace play { namespace robust { namespace net {

class sodium_cipher : public codec
{
public:
  struct exception : public std::exception
  {
    exception(const std::string& what)
        : what_{what}
    {
    }

    const char* what() const noexcept override
    {
      return what_.c_str();
    }

    std::string what_;
  };

public:
  sodium_cipher(size_t handle, sodium_handshake& handshake)
      : handle_{handle},
        handshake_{handshake}
  {
  }

  // dst에 대해 prepare하고 암호화 진행
  /**
   * @throws 암호화 실패 시 예외 발생
   */
  size_t encode(const asio::const_buffer& src, asio::streambuf& dst);

  // 미리 할당된 버퍼를 사용하여 변환.
  /**
   * sodium chacha20은 in-place 변환이 가능하여 미리 할당이 가능
   * @return 사용한 바이트 수를 지정 
   */
  size_t encode(const asio::const_buffer& src, asio::mutable_buffer& dst);

  // decrypt src_buf with internal buffer and put it into const_buffer
  /**
   * @return <consumed_len, frame buffer>를 돌려줌
   * @throws 복호화 실패 시 예외 발생
   */
  std::tuple<size_t, asio::const_buffer> decode(const asio::const_buffer& src);

private:
  size_t handle_;
  sodium_handshake& handshake_;
};

}}}  // namespace play::robust::net

#include <play/robust/net/protocol/sodium_cipher.ipp>