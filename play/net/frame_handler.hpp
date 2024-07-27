#pragma once

#include <play/net/asio.hpp>
#include <play/net/runner.hpp>

#include <memory>

namespace play {

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
  frame_handler(runner& runner)
      : runner_{runner}
  {
  }

  virtual void established(session_ptr se) = 0;

  virtual void closed(session_ptr se, error_code ec) = 0;

  virtual void recv(session_ptr se, topic pic, const void* data, size_t len) = 0;

  runner& get_runner()
  {
    return runner_;
  }

private:
  runner& runner_;
};

// default 핸들러로 client/server의 인터페이스를 추가하기 위해 사용
template <typename Session, typename Topic, typename Frame>
class frame_void_handler : public frame_handler<Session, Topic, Frame>
{
  using frame_handler = frame_handler<Session, Topic, Frame>;
  using topic = Topic;
  using session_ptr = std::shared_ptr<Session>;

public:
  frame_void_handler(runner& runner)
      : frame_handler{runner}
  {
  }

  void established(session_ptr se) override {}

  void closed(session_ptr se, error_code ec) override {}

  // do nothing empty handler
  void recv(session_ptr, topic, const void*, size_t) override {}
};

// 세션 핸들러의 타잎 정의에서 추론하여 아무것도 하지 않는 프레임핸들러를 만듦
/**
 * session_handler 인터페이스 구현한 client와 server에서 직접 메세지를 
 * 처리하려고 할 때 runner에 대한 간단한 wrapper로만 사용하기 위해 필요.
 * 
 * frame_handler를 참조로만 갖고 안전하게 쓰고 싶어 항상 생성자에 전달하면서 필요해짐
 */
template <typename SessionHandler>
class frame_default_handler
    : public frame_void_handler<typename SessionHandler::session, typename SessionHandler::topic,
                                typename SessionHandler::frame>
{
  using base = frame_void_handler<typename SessionHandler::session, typename SessionHandler::topic,
                                  typename SessionHandler::frame>;

public:
  frame_default_handler(runner& runner)
      : base{runner}
  {
  }
};

}  // namespace play
