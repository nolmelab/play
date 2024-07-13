#include <play/robust/net/protocol/length_delimited.hpp>

namespace play { namespace robust { namespace net {

inline std::optional<codec::const_buffer> length_delimited::decode(const const_buffer& src_buf)
{
  if (src_buf.size() < length_field_size)
  {
    return {};
  }

  const char* p = reinterpret_cast<const char*>(src_buf.data());
  size_t len = 0;

  for (int i = 0; i < length_field_size; ++i)
  {
    len = len | (p[i] << i * 8);
  }

  // src_buf has the payload
  if (src_buf.size() >= len)
  {
    return const_buffer{p + length_field_size, len};
  }

  // not sufficient bytes for the payload
  return {};
}

inline size_t length_delimited::encode(const const_buffer& src_buf,
                                       asio::streambuf& dest_stream_buf)
{
  const size_t total_len = length_field_size + src_buf.size();
  auto dest_buf = dest_stream_buf.prepare(total_len);

  size_t len = src_buf.size();
  char* p = reinterpret_cast<char*>(dest_buf.data());

  for (int i = 0; i < length_field_size; ++i)
  {
    p[i] = len & 0xFF;
    len >>= 8;
  }

  // copy bytes
  char* dest = p + length_field_size;
  const char* src = reinterpret_cast<const char*>(src_buf.data());
  std::memcpy(dest, src, src_buf.size());

  // make the payload available for reading
  dest_stream_buf.commit(total_len);

  return src_buf.size() + length_field_size;
}

}}}  // namespace play::robust::net