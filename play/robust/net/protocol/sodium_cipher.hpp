#pragma once

#include <optional>
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

  // encrypt src_buf into dest_buf
  /**
   * @throws 암호화 실패 시 예외 발생
   */
  size_t encode(const asio::const_buffer& src, asio::stream_buf& dst);

  // decrypt src_buf with internal buffer and put it into const_buffer
  /**
   * @return <consumed_len, frame buffer>를 돌려줌
   * @throws 복호화 실패 시 예외 발생
   */
  std::tuple<size_t, asio::const_buffer> decode(const asio::const_buffer& src);

private:
  size_t handle_;
  sodium_handshake& handshake_;
  asio::streambuf decode_buf_;
  static inline thread_local asio::streambuf encode_buf_;
};

}}}  // namespace play::robust::net