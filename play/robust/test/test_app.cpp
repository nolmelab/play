#include <doctest/doctest.h>
#include <play/robust/app/act.hpp>
#include <play/robust/app/actor.hpp>
#include <play/robust/app/app.hpp>

using namespace play::robust::app;

namespace {

class dummy_act : public act
{
public:
  dummy_act(actor& owner)
      : act(owner, typeid(dummy_act).name())
  {
  }

  bool activate() final
  {
    return true;
  }

  void deactivate() final {}

  void destroy() final {}
};

class dummy_actor : public actor
{
public:
  dummy_actor()
  {
  }

  bool activate() final
  {
    return true;
  }

  void deactivate() final {}

  void destroy() final {}
};

}  // namespace

TEST_CASE("app")
{
  // app은 골격만 있어 테스트할 내용이 매우 적다.
  SUBCASE("actor")
  {
    auto actor_1 = std::make_shared<dummy_actor>();
    auto da = actor_1->create_act<dummy_act>();
    CHECK(actor_1->get_act<dummy_act>());
  }

  SUBCASE("app")
  {
    app& app = app::get();
    CHECK(app.start(R"({"port" : 7000, "concurrency" : 8 })"));
    app.stop();
  };
}