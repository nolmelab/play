#include <doctest/doctest.h>
#include <play/base/logger.hpp>
#include <play/net/runner.hpp>
#include <play/net/runner/poll_runner.hpp>
#include <play/net/runner/thread_runner.hpp>

using namespace play;

TEST_CASE("runner")
{
  SUBCASE("io_context")
  {
    auto func = []()
    {
      LOG()->info("hello boost::asio::post from context");
    };

    asio::io_context context;

    asio::post(context, func);
    context.run();

    context.reset();

    asio::post(context, func);
    context.run();
  }

  SUBCASE("poll")
  {
    auto func = []()
    {
      LOG()->info("hello boost::asio::post from poll_runner");
    };

    poll_runner runner;

    runner.post(func);
    runner.poll();

    runner.post(func);
    runner.poll();
  }

  SUBCASE("thread")
  {
    thread_runner runner;

    auto func = []()
    {
      LOG()->info("hello from thread_runner");
    };

    runner.post(func);
    runner.post(func);
    runner.post(func);
    runner.post(func);

    runner.sleep(10);

    runner.stop();
  }
}
