#pragma once

#include <optional>
#include <play/robust/net/codec.hpp>
#include <play/robust/net/protocol/sodium_handshaker.hpp>

namespace play { namespace robust { namespace net {

class sodium_cipher : public codec
{
 public:
  struct exception : public std::exception
  {
    exception(const std::string& what) : what_{what} {}

    const char* what() const noexcept override { return what_.c_str(); }

    std::string what_;
  };

 public:
  sodium_cipher(sodium_handshaker& handshaker) : handshaker_{handshaker} {}

  // decrypt src_buf with internal buffer and put it into const_buffer
  /**
   * @throws 복호화 실패 시 예외 발생
   */
  std::optional<const_buffer> decode(const const_buffer& src_buf);

  // encrypt src_buf into dest_buf
  /**
   * @throws 암호화 실패 시 예외 발생
   */
  size_t encode(const const_buffer& src_buf, asio::streambuf& dest_stream_buf);

 private:
  sodium_handshaker& handshaker_;
  asio::streambuf decode_buf_;
  static inline thread_local asio::streambuf encode_buf_;
};

}}}  // namespace play::robust::net