#pragma once

#include <play/robust/net/asio.hpp>

#include <memory>

namespace play { namespace robust { namespace net {

// 프레임 바이트를 받아서 Frame 포인터를 만들어 전달하는 클래스
/**
 * - subscribe handler with topic 
 * - regiser creator prototype functions for each topic 
 */
template <typename Topic, typename Frame>
class frame_handler
{
public:
  using topic = Topic;
  using ptr = std::shared_ptr<Frame>;

public:
  frame_handler() = default;

  virtual void recv(session_ptr se, topic pic, const void* data, size_t len) = 0;
};

}}}  // namespace play::robust::net
