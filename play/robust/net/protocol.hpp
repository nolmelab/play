#pragma once

#include <functional>
#include <play/robust/net/asio.hpp>

namespace play { namespace robust { namespace net {

// type used to mark empty topic (topic not used)
struct empty_topic
{
};

template <typename Topic>
class protocol
{
public:
  using topic = Topic;

public:
  protocol() {}

  // 비트 연산을 제공하는 값 타잎들에 대해 직렬화
  template <typename Value>
  void serialize(asio::mutable_buffer& mbuf, Value v);

  void serialize(asio::mutable_buffer& mbuf, bool v);

  // 비트 연산을 제공하는 값 타잎들에 대해 역직렬화
  template <typename Value>
  void deserialize(const asio::const_buffer& cbuf, Value& v);

  void deserialize(const asio::const_buffer& cbuf, bool& v);
};

}}}  // namespace play::robust::net

#include <play/robust/net/protocol.ipp>