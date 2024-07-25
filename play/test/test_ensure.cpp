#include <doctest/doctest.h>
#include <play/ensure/act.hpp>
#include <play/ensure/act_factory.hpp>
#include <play/ensure/act_parallel.hpp>
#include <play/ensure/act_serial.hpp>
#include <play/ensure/bot.hpp>
#include <play/ensure/ensure.hpp>
#include <play/ensure/flow.hpp>

using namespace play::ensure;

namespace {

class mockup_actor : public ::play::app::actor
{
public:
  bool start() final
  {
    return true;
  }

  void stop() final {}
};

}  // namespace

TEST_CASE("ensure")
{
  SUBCASE("act_factory")
  {
    PLAY_REG_ACT(act_serial);
    PLAY_REG_ACT(act_parallel);

    mockup_actor owner;
    auto ap = act_factory::get().create("act_serial", owner, {}, "{}", "test");
    CHECK(!!ap);
    CHECK(ap->get_name() == "test");
    CHECK(std::addressof(ap->get_owner()) == std::addressof(owner));

    auto ap2 = act_factory::get().create("act_none", owner, {}, "{}", "test");
    CHECK(!ap2);
  }

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

  SUBCASE("act_serial") {}
}