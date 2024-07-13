#include <doctest/doctest.h>
#include <play/robust/base/logger.hpp>
#include <play/robust/net/runner/thread_runner.hpp>

using namespace play::robust::net;

TEST_CASE("timer")
{
  SUBCASE("basics")
  {
    asio::io_context io;
    timer_service ts{io};

    ts.once(std::chrono::milliseconds(300),
            [](timer&)
            {
              LOG()->info("timer once called");
            });

    io.run();

    io.restart();

    auto t1 = ts.repeat(std::chrono::milliseconds(100),
                        [](timer& timer)
                        {
                          LOG()->info("timer repeat called");
                          timer.cancel();
                        });

    io.run();
  }
}