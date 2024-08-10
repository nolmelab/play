#pragma once

#include <functional>
#include <play/net/asio.hpp>

namespace play {

// type used to mark empty topic (topic not used)
struct empty_topic
{
};

// protocol 기반 클래스. Topic은 Topic{1}이 되고 비트 연산을 지원하는 숫자 타잎
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
