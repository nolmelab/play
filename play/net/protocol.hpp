#pragma once

#include <functional>
#include <play/net/asio.hpp>

namespace play {

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
  inline static size_t max_send_size = 1024 * 1024;
  inline static size_t max_recv_size = 1024 * 1024;

public:
  protocol() {}
};

}  // namespace play
