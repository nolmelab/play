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
};

}}}  // namespace play::robust::net
