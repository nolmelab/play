#include <doctest/doctest.h>

#include <play/base/logger.hpp>

/// test logger 
/**
 * checks with console or system*.log file under logs folder 
 */
TEST_CASE("test_logger")
{
  SUBCASE("basic usage")
  {
    LOG()->set_level(spdlog::level::trace);

    LOG()->trace("hello playground logger trace");
    LOG()->debug("hello playground logger debug");
    LOG()->info("hello playground logger info");
    LOG()->warn("hello playground logger warn");
    LOG()->error("hello playground logger error");

    LOG()->flush();
  }

  SUBCASE("change log level")
  {
    LOG()->set_level(spdlog::level::trace);
    LOG()->trace("1");

    LOG()->set_level(spdlog::level::debug);
    LOG()->trace("2");

    LOG()->set_level(spdlog::level::trace);
    LOG()->trace("3");

    LOG()->flush();
  }

  SUBCASE("log time test")
  {
    LOG()->info("first");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    LOG()->info("second");
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    LOG()->info("third");

    LOG()->flush();
  }

  SUBCASE("thread id test")
  {
    LOG()->info("from main");

    auto thread = std::thread([](){
      LOG()->info("from thread");
    });

    thread.join();
  } 
}