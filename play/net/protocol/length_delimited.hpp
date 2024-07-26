#pragma once

#include <optional>
#include <play/net/asio.hpp>
#include <play/net/codec.hpp>

namespace play {

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
  size_t encode(const asio::const_buffer& src, asio::streambuf& dst);

  // src의 길이를 dst에 쓰고 그 내용도 dst에 쓴다.
  size_t encode(const asio::const_buffer& src, asio::mutable_buffer& dst);

  // src_buf에서 길이만큼에 해당 되는 payload를 읽어 const_buffer로 포인터를 맞춰 돌려준다
  asio::const_buffer decode(const asio::const_buffer& src);

private:
  size_t handle_;
};

}  // namespace play::net

#include <play/net/protocol/length_delimited.ipp>