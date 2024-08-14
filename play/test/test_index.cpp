#include <doctest/doctest.h>
#include <map>
#include <play/app/index.hpp>
#include <string>

TEST_CASE("index")
{

  SUBCASE("usage")
  {
    std::map<int, std::string> container;
    play::index<std::map<int, std::string>, size_t> index{container};
    container.insert({1, std::string{"hello"}});
    index.link(10, 1);
    // found
    {
      auto iter = index.find(10);
      CHECK(iter->second == std::string{"hello"});
    }
    // not found
    {
      index.unlink(10);
      auto iter = index.find(10);
      CHECK(iter == container.end());
    }
  }
}