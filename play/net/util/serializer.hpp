#pragma once

#include <cstddef>
#include <cstdint>
#include <play/base/macros.hpp>

namespace play {

class serializer
{
public:
  // 비트 연산을 제공하는 값 타잎들에 대해 직렬화
  template <typename Value>
  static void serialize(uint8_t* buf, size_t len, Value v);

  static void serialize(uint8_t* buf, size_t len, bool v);

  // 비트 연산을 제공하는 값 타잎들에 대해 역직렬화
  template <typename Value>
  static void deserialize(const uint8_t* buf, size_t len, Value& v);

  static void deserialize(const uint8_t* buf, size_t len, bool& v);
};

template <typename Value>
inline void serializer::serialize(uint8_t* buf, size_t len, Value v)
{
  PLAY_CHECK(len >= sizeof(Value));

  auto sv = v;

  for (int i = 0; i < sizeof(Value); ++i)
  {
    buf[i] = sv & 0xFF;
    sv >>= 8;
  }
}

inline void serializer::serialize(uint8_t* buf, size_t len, bool v)
{
  uint8_t tv = v ? 1 : 0;
  buf[0] = tv;
}

template <typename Value>
inline void serializer::deserialize(const uint8_t* buf, size_t len, Value& v)
{
  PLAY_CHECK(len >= sizeof(Value));
  v = 0;

  for (int i = 0; i < sizeof(Value); ++i)
  {
    Value bv = buf[i];
    v |= ((bv & 0xFF) << i * 8);
  }
}

inline void serializer::deserialize(const uint8_t* buf, size_t len, bool& v)
{
  PLAY_CHECK(len > 0);
  uint8_t tv = buf[0];
  v = tv == 0 ? false : true;
}

}  // namespace play