#include <doctest/doctest.h>
#include <play/app/act.hpp>
#include <play/app/actor.hpp>

using namespace play;

namespace {

class dummy_act : public act
{
public:
  dummy_act(actor& owner)
      : act(owner)
  {
  }
};

class dummy_actor : public actor
{
public:
  dummy_actor() {}

private:
  bool on_start() final
  {
    return true;
  }

  // 종료
  void on_stop() final
  {
    //
  }
};

}  // namespace

TEST_CASE("app")
{
  // app은 골격만 있어 테스트할 내용이 매우 적다.
  SUBCASE("actor")
  {
    auto actor_1 = std::make_shared<dummy_actor>();
    auto da = actor_1->create_act<dummy_act>();
    auto da2 = actor_1->get_act<dummy_act>();
  }
}