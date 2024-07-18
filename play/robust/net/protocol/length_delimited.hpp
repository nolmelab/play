#pragma once

#include <optional>
#include <play/robust/net/asio.hpp>
#include <play/robust/net/codec.hpp>

namespace play { namespace robust { namespace net {

// 페이로드 길이를 앞에 32비트 갖는 코덱이다.
class length_delimited : public codec
{
public:
  static const size_t length_field_size = 4;

public:
  length_delimited(size_t handle = 0)
      : handle_{handle}
  {
  }

  // src 버퍼 내용을 dst 버퍼에 쓴다. dst에 대해 prepare(), commit()을 한다
  /**
   * @throws 암호화 실패 시 예외 발생
   */
  size_t encode(const asio::const_buffer& src, asio::streambuf& dst);

  // sbuf의 길이를 dest_buf에 쓰고, 그 내용도 dest_buf에 쓴다.
  /**
   * @throws 암호화 실패 시 예외 발생
   */
  size_t encode(const asio::const_buffer& src, asio::mutable_buffer& dest);

  // src_buf에서 길이만큼에 해당 되는 body를 읽어 const_buffer로 포인터를 맞춰 돌려준다
  asio::const_buffer decode(const asio::const_buffer& src);

  // TODO: add encode using asio::streambuf

private:
  size_t handle_;
};

}}}  // namespace play::robust::net

#include <play/robust/net/protocol/length_delimited.ipp>