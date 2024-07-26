#include <doctest/doctest.h>
#include <play/base/string_util.hpp>

using namespace play;

TEST_CASE("string_util")
{
  //
  SUBCASE("split strings")
  {
    {
      auto result = string_util::split("a/b/c//d /", "/");
    }

    {
      auto result = string_util::split("/", "/");
      CHECK(result == std::vector<std::string>{});
    }

    {
      auto result = string_util::split("a/", "/");
      CHECK(result == std::vector{std::string{"a"}});
    }
  }

  SUBCASE("find ")
  {
    std::string s{"a/b/c//"};
    auto p1 = s.find_first_of("/");
    CHECK(p1 == 1);
    auto p2 = s.find_first_of("/", p1 + 1);
    CHECK(p2 == 3);
    auto p3 = s.find_first_of("/", p2 + 1);
    CHECK(p3 == 5);
    auto p4 = s.find_first_of("/", p3 + 1);
    CHECK(p4 == 6);
  }
}
