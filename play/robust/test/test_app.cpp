#include <doctest/doctest.h>
#include <play/robust/app/act.hpp>
#include <play/robust/app/actor.hpp>
#include <play/robust/app/app.hpp>

using namespace play::robust::app;

namespace {

class dummy_act : public act
{
public:
  bool activate() final
  {
    return true;
  }

  void deactivate() {}

  void destroy() {}
};

}  // namespace

TEST_CASE("app")
{
  // app은 골격만 있어 테스트할 내용이 매우 적다.
  SUBCASE("actor")
  {
    auto actor_1 = std::make_shared<actor>("actor");
    auto da = actor_1->create_act<dummy_act>();
    CHECK(actor_1->get_act<dummy_act>());
  }

  SUBCASE("service") {}
}