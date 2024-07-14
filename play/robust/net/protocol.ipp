#include <play/robust/base/macros.hpp>
#include <play/robust/net/protocol.hpp>

namespace play { namespace robust { namespace net {

template <typename Topic, typename Adapter>
template <typename Value>
inline void protocol<Topic, Adapter>::serialize(asio::mutable_buffer& mbuf, Value v)
{
  PLAY_CHECK(mbuf.size() >= sizeof(Value));

  char* p = reinterpret_cast<char*>(mbuf.data());
  auto sv = v;

  for (int i = 0; i < sizeof(Value); ++i)
  {
    p[i] = sv & 0xFF;
    sv >>= 8;
  }
}

template <typename Topic, typename Adapter>
inline void protocol<Topic, Adapter>::serialize(asio::mutable_buffer& mbuf, bool v)
{
  uint8_t tv = v ? 1 : 0;
  serialize(mbuf, tv);
}

template <typename Topic, typename Adapter>
template <typename Value>
inline void protocol<Topic, Adapter>::deserialize(const asio::const_buffer& cbuf, Value& v)
{
  const char* p = reinterpret_cast<const char*>(cbuf.data());
  v = 0;

  for (int i = 0; i < sizeof(Value); ++i)
  {
    v |= (p[i] << i * 8);
  }
}

template <typename Topic, typename Adapter>
inline void protocol<Topic, Adapter>::deserialize(const asio::const_buffer& cbuf, bool& v)
{
  uint8_t tv = 0;
  deserialize(cbuf, tv);
  v = tv == 0 ? false : true;
}

}}}  // namespace play::robust::net