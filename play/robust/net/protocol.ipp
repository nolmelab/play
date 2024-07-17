#include <play/robust/base/macros.hpp>
#include <play/robust/net/protocol.hpp>

namespace play { namespace robust { namespace net {

template <typename Topic>
template <typename Value>
inline void protocol<Topic>::serialize(char* buf, size_t len, Value v)
{
  PLAY_CHECK(len >= sizeof(Value));

  auto sv = v;

  for (int i = 0; i < sizeof(Value); ++i)
  {
    buf[i] = sv & 0xFF;
    sv >>= 8;
  }
}

template <typename Topic>
inline void protocol<Topic>::serialize(char* buf, size_t len, bool v)
{
  uint8_t tv = v ? 1 : 0;
  serialize(buf, len, tv);
}

template <typename Topic>
template <typename Value>
inline void protocol<Topic>::deserialize(const char* buf, size_t len, Value& v)
{
  PLAY_CHECK(len >= sizeof(Value));
  v = 0;

  for (int i = 0; i < sizeof(Value); ++i)
  {
    v |= (buf[i] << i * 8);
  }
}

template <typename Topic>
inline void protocol<Topic>::deserialize(const char* buf, size_t len, bool& v)
{
  uint8_t tv = 0;
  deserialize(buf, len, tv);
  v = tv == 0 ? false : true;
}

}}}  // namespace play::robust::net