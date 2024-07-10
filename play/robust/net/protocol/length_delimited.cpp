#include <play/robust/net/protocol/length_delimited.hpp>

namespace play { namespace robust { namespace net {

std::optional<codec::const_buffer> length_delimited::decode(const const_buffer& src_buf)
{
  if (src_buf.size() < length_field_size_)
  {
    return {};
  }

  const char* p = reinterpret_cast<const char*>(src_buf.data());
  size_t len = 0; 

  for (int i = 0; i < length_field_size_; ++i)
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

  for (int i=0; i<length_field_size_; ++i)
  {
    p[i] = len & 0xFF;
    len >>= 8;
  }

  // copy bytes
}

} } } // play::robust::net