#include <doctest/doctest.h>
#include <play/ensure/act.hpp>

using namespace play::ensure;

TEST_CASE("ensure")
{
  SUBCASE("act::path")
  {
    SUBCASE("basics")
    {
      act::path p1;
      p1.setup("/a/b");
      CHECK(p1.full_path_ == "/a/b");
      CHECK(p1.act_name_ == "b");
      CHECK(p1.parts_ == std::vector<std::string>{"a", "b"});
      CHECK(act::path::is_absolute_path(p1.full_path_));
      CHECK(!act::path::is_absolute_path(p1.act_name_));
      CHECK(act::path::is_relative_path(p1.act_name_));
      CHECK(act::path::get_first_act(p1.full_path_) == "a");
      CHECK(act::path::get_last_act(p1.full_path_) == "b");
      CHECK(act::path::pop_head_act(p1.full_path_) == "b");
    }

    SUBCASE("child")
    {
      act::path p1;
      p1.setup("/aa/bb/cc/dd");
      act::path p2;
      p2.setup("/aa/bb");
      CHECK(act::path::is_child_of(p2.full_path_, p1.full_path_));
      CHECK(act::path::get_child_path(p2.full_path_, p1.full_path_) == "cc/dd");
    }
  }
}