#include <play/robust/net/protocol/length_delimited.hpp>

namespace play { namespace robust { namespace net {

const size_t length_field_size = 4;

std::optional<codec::const_buffer> length_delimited::decode(const const_buffer& src_buf)
{
  if (src_buf.size() < length_field_size)
  {
    return {};
  }

  const char* p = reinterpret_cast<const char*>(src_buf.data());
  size_t len = 0; 

  for (int i = 0; i < length_field_size; ++i)
  {
    len = len << 8 | p[i];
  }

  // src_buf has the payload
  if (src_buf.size() >= len)
  {
    return const_buffer{ p + length_field_size_, len };
  }

  // not sufficient bytes for the payload 
  return {};
}

size_t length_delimited::encode(const const_buffer& src_buf, mutable_buffer& dest_buf)
{
  size_t len = src_buf.size();

  char* p = reinterpret_cast<char*>(dest_buf.data());

  for (int i=0; i<length_field_size; ++i)
  {
    p[i] = len & 0xFF;
    len >>= 8;
  }

  // copy bytes
  char* dest = p + length_field_size; 
  const char* src = reinterpret_cast<const char*>(src_buf.data());
  std::memcpy(dest, src, src_buf.size());
}

} } } // play::robust::net