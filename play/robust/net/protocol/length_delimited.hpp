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
  length_delimited(size_t handle) : handle_{handle} {}

  // get one frame from src_buf and put it into const_buffer
  /**
   * 돌려주는 버퍼는 src_buf의 내부 메모리 중 길이 필드를 제외하고 페이로드 만큼
   * 페이로드가 부족하면 std::optional의 has_value()가 false
   */
  std::optional<const_buffer> decode(const const_buffer& src_buf);

  // put length field into dest_buf with src_buf content as payload
  /**
   * dest_buf에 대해 prepare로 메모리를 확보
   */
  size_t encode(const const_buffer& src_buf, asio::streambuf& dest_stream_buf);

 private:
  size_t handle_;
};

}}}  // namespace play::robust::net

#include <play/robust/net/protocol/length_delimited.ipp>