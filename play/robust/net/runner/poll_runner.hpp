#pragma once

#include <play/robust/net/runner.hpp>

namespace play { namespace robust { namespace net {

// io_context runner base class
/**
 * poll() 함수를 통해 io_context를 통해 실행
 */
class poll_runner : public runner
{
public:
  poll_runner() = default;

  size_t poll()
  {
    get_ioc().restart();  // stopped_ 상태를 false로 만들어 실행 가능하게 함

    return get_ioc().run();
  }
};

}}}  // namespace play::robust::net