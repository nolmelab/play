#include <doctest/doctest.h>
#include <play/base/logger.hpp>
#include <play/net/runner/thread_runner.hpp>

using namespace play::net;

TEST_CASE("timer")
{
  SUBCASE("basics")
  {
    asio::io_context io;
    timer_service ts{io};

    ts.once(std::chrono::milliseconds(300),
            [](timer&)
            {
              LOG()->info("timer once called 1");
            });

    ts.once(std::chrono::milliseconds(300),
            [](timer&)
            {
              LOG()->info("timer once called 2");
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

  SUBCASE("strand")
  {
    asio::io_context io;
    asio::io_context::strand strand{io};
    timer_service ts{io};

    for ( int i=0; i<100; ++i)
    {
    ts.once(strand, std::chrono::milliseconds(300),
            [i](timer&)
            {
              LOG()->info("timer once called {}", i);
            });
    }

    // 쓰레드 2개로는 정확하게 알기 어렵지만 동작한다.
    auto thread = std::thread([&io]() { io.run();});
    io.run();

    thread.join();

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