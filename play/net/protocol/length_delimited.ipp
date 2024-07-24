#include <play/base/macros.hpp>
#include <play/net/protocol/length_delimited.hpp>
#include <play/net/util/serializer.hpp>

namespace play { namespace net {

inline size_t length_delimited::encode(const asio::const_buffer& src, asio::streambuf& dst)
{
  PLAY_CHECK(src.size() > 0);

  auto total_len = length_field_size + src.size();
  auto dst_buf = dst.prepare(total_len);
  auto wbuf = reinterpret_cast<char*>(dst_buf.data());
  auto mbuf = asio::mutable_buffer{reinterpret_cast<void*>(wbuf), total_len};
  auto sbuf = encode(src, mbuf);
  dst.commit(total_len);
  return total_len;
}

inline size_t length_delimited::encode(const asio::const_buffer& src, asio::mutable_buffer& dst)
{
  PLAY_CHECK(src.size() > 0);
  PLAY_CHECK(dst.size() >= src.size());

  const size_t total_len = length_field_size + src.size();
  PLAY_CHECK(dst.size() >= total_len);

  size_t len = src.size();
  char* p = reinterpret_cast<char*>(dst.data());

  for (int i = 0; i < length_field_size; ++i)
  {
    p[i] = len & 0xFF;
    len >>= 8;
  }

  // copy bytes to the dst frame position
  char* wdst = p + length_field_size;
  const char* csrc = reinterpret_cast<const char*>(src.data());
  std::memcpy(wdst, csrc, src.size());

  return total_len;
}

inline asio::const_buffer length_delimited::decode(const asio::const_buffer& src)
{
  if (src.size() < length_field_size)
  {
    return {};
  }

  const char* p = reinterpret_cast<const char*>(src.data());
  size_t len = 0;

  for (int i = 0; i < length_field_size; ++i)
  {
    uint32_t pv = p[i];
    len |= ((pv & 0xFF) << i * 8);
  }

  // src_buf has the payload
  if (src.size() >= (len + length_field_size))
  {
    return asio::const_buffer{p + length_field_size, len};
  }

  // not sufficient bytes for the payload
  return {};
}

}}  // namespace play::net