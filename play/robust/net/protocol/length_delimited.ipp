#include <play/robust/base/macros.hpp>
#include <play/robust/net/protocol/length_delimited.hpp>

namespace play { namespace robust { namespace net {

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
    len = len | (p[i] << i * 8);
  }

  // src_buf has the payload
  if (src.size() >= (len + length_field_size))
  {
    return asio::const_buffer{p + length_field_size, len};
  }

  // not sufficient bytes for the payload
  return {};
}

inline size_t length_delimited::encode(const asio::const_buffer& src, asio::mutable_buffer& dest,
                                       bool header_only)
{
  const size_t total_len = length_field_size + src.size();
  PLAY_CHECK(dest.size() >= total_len);

  size_t len = src.size();
  char* p = reinterpret_cast<char*>(dest.data());

  for (int i = 0; i < length_field_size; ++i)
  {
    p[i] = len & 0xFF;
    len >>= 8;
  }

  if (!header_only)
  {
    // copy bytes
    char* dest = p + length_field_size;
    const char* csrc = reinterpret_cast<const char*>(src.data());
    std::memcpy(dest, csrc, src.size());
  }

  return total_len;
}

}}}  // namespace play::robust::net