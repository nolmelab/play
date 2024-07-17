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

  // get one frame from src_buf and put it into const_buffer
  /**
   * src_buf에서 길이만큼에 해당 되는 body를 읽어 const_buffer로 포인터를 맞춰 돌려줌
   * @param src_buf 받은 데이터
   * @return 길이 필드만큼 데이터가 있으면 해당 지점을 담아서 돌려줌. 
   */
  asio::const_buffer decode(const asio::const_buffer& src);

  // sbuf의 길이를 dest_buf에 쓰고, 그 내용도 dest_buf에 쓴다.
  /**
   * @param pic 전송할 토픽
   * @param src 전송할 내용
   * @param dest 목적지 버퍼는 미리 sbuf.size() + length_field_size만큼 확보해야 함
   * @param header_only true이면 길이 필드만 dest_buf의 앞에 쓴다.
   * @return src_buf.size() + length_field_size를 돌려줌
   */
  size_t encode(const asio::const_buffer& src, asio::mutable_buffer& dest,
                bool header_only = false);

private:
  size_t handle_;
};

}}}  // namespace play::robust::net

#include <play/robust/net/protocol/length_delimited.ipp>