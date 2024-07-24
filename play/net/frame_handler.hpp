#pragma once

#include <play/net/asio.hpp>

#include <memory>

namespace play { namespace net {

// client와 server의 하위 클래스에서 처리할 경우 표시를 하기 위한 frame 클래스
struct frame_subclass
{
};

// 프레임 바이트를 받아서 Frame 포인터를 만들어 전달하는 클래스
/**
 * - subscribe handler with topic 
 * - regiser creator prototype functions for each topic 
 */
template <typename Session, typename Topic, typename Frame>
class frame_handler
{
public:
  using topic = Topic;
  using frame_ptr = std::shared_ptr<Frame>;
  using session_ptr = std::shared_ptr<Session>;

public:
  frame_handler() = default;

  virtual void recv(session_ptr se, topic pic, const void* data, size_t len) = 0;
};

template <typename Session, typename Topic, typename Frame>
class frame_subclass_handler : public frame_handler<Session, Topic, frame_subclass>
{
  using self = frame_subclass_handler<Session, Topic, Frame>;
  using topic = Topic;
  using session_ptr = std::shared_ptr<Session>;

public:
  // do nothing empty handler
  void recv(session_ptr, topic, const void*, size_t) override {}

  inline static self& get()
  {
    static self instance_;
    return instance_;
  }
};

}}  // namespace play::net
