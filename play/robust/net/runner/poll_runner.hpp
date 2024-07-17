#pragma once

#include <play/robust/net/runner.hpp>

namespace play { namespace robust { namespace net {

// io_context runner base class
/**
 * poll() 함수를 통해 io_context를 실행
 */
class poll_runner : public runner
{
public:
  poll_runner(const std::string& name = "default")
      : runner(name)
  {
  }

  size_t poll()
  {
    get_ioc().restart();  // stopped_ 상태를 false로 만들어 실행 가능하게 함
    return get_ioc().run();
  }

  size_t poll_one()
  {
    get_ioc().restart();
    return get_ioc().poll_one();
  }
};

}}}  // namespace play::robust::net