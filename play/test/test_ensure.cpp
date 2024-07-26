#include <doctest/doctest.h>
#include <play/ensure/act.hpp>
#include <play/ensure/act_factory.hpp>
#include <play/ensure/act_parallel.hpp>
#include <play/ensure/act_serial.hpp>
#include <play/ensure/acts/act_message.hpp>
#include <play/ensure/bot.hpp>
#include <play/ensure/ensure.hpp>
#include <play/ensure/flow.hpp>
#include <play/net/runner.hpp>

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
    PLAY_REGISTER_ACT(act_serial);
    PLAY_REGISTER_ACT(act_parallel);

    mockup_actor owner;
    auto ap = act_factory::get().create("act_serial", owner, {}, "{}", "test");
    CHECK(!!ap);
    CHECK(ap->get_name() == "test");
    CHECK(std::addressof(ap->get_owner()) == std::addressof(owner));

    auto ap2 = act_factory::get().create("act_none", owner, {}, "{}", "test");
    CHECK(!ap2);
  }

  SUBCASE("act_serial")
  {
    // act_composite requires flow
    std::string conf = R"(
    {
    "flow" : [
      {
      "type" : "act_message", 
      "name" : "message_1",
      "message" : "hello ensure! I'm the first act",
      "slots" : { 
        "success" : { "cmd" : "next" }, 
        "fail" : { "cmd" : "next" }
        }
      },
      {
      "type" : "act_message", 
      "name" : "message_2",
      "message" : "hello ensure! I'm the second act",
      "slots" : { 
        "success" : { "cmd" : "exit" }, 
        "fail" : { "cmd" : "exit" }
        }
      }
    ]
    }
    )";

    PLAY_REGISTER_ACT(act_message);
    auto jserial = nlohmann::json::parse(conf);
    mockup_actor owner;
    auto ap = act_factory::get().create("act_serial", owner, {}, jserial, "test");
    CHECK(ap->activate());
    ap->update();  // to signal from act_message
    ap->deactivate();
  }

  SUBCASE("flow and jump")
  {
    std::string conf = R"(
    {
    "flow": [
      {
        "type" : "act_serial", 
        "name" : "serial_1",
        "flow" : 
        [
          {
            "type" : "act_message", 
            "name" : "message_1",
            "message" : "hello ensure! I'm the first act",
            "slots" : { 
              "success" : { "cmd" : "jump", "target" : "/flow/message_top_1" }, 
              "fail" : { "cmd" : "next" } }
          },
          {
            "type" : "act_message", 
            "name" : "message_2",
            "message" : "hello ensure! I'm the second act",
            "slots" : { 
              "success" : { "cmd" : "exit" }, 
              "fail" : { "cmd" : "exit" } }
          }
        ]
      },
      {
      "type" : "act_message", 
      "name" : "message_top_1",
      "message" : "hello ensure! I'm the first top act",
      "slots" : { 
        "success" : { "cmd" : "next" }, 
        "fail" : { "cmd" : "next" }
        }
      },
      {
      "type" : "act_message", 
      "name" : "message_top_2",
      "message" : "hello ensure! I'm the second top act",
      "slots" : { 
        "success" : { "cmd" : "jump", "target" : "serial_1" }, 
        "fail" : { "cmd" : "exit" }
        }
      }
    ]
    }
    )";

    PLAY_REGISTER_ACT(act_message);
    auto jflow = nlohmann::json::parse(conf);

    mockup_actor owner;
    act::ptr fp = std::make_shared<flow>(owner, jflow);

    // activate loads from json
    CHECK(fp->activate());

    SUBCASE("find")
    {
      CHECK(fp->find("/flow")->get_name() == "flow");
      CHECK(fp->find("/flow/message_top_1")->get_name() == "message_top_1");
      CHECK(fp->find("/flow/message_top_2")->get_name() == "message_top_2");
      CHECK(fp->find("/flow/serial_1/message_1")->get_name() == "message_1");

      auto serial = fp->find("/flow/serial_1");
      CHECK(!!serial);

      SUBCASE("full path")
      {
        auto message_2 = serial->find("/flow/serial_1/message_2");
        CHECK(message_2->get_name() == "message_2");
      }

      SUBCASE("relative path including self")
      {
        auto message_2 = serial->find("serial_1/message_2");
        CHECK(message_2->get_name() == "message_2");
      }

      SUBCASE("relative path excluding self")
      {
        auto message_2 = serial->find("message_2");
        CHECK(message_2->get_name() == "message_2");
      }

      CHECK(fp->find("/flow/serial_1/message_2")->get_name() == "message_2");
      CHECK(!fp->find("/flow/serial_1/message_x"));
      CHECK(!fp->find("/"));
    }

    SUBCASE("jump")
    {
      CHECK(fp->find("/flow/serial_1")->is_active());
      CHECK(fp->find("/flow/serial_1/message_1")->is_active());
      fp->update();  // message_1 update. jump to message_top_1
      CHECK(!fp->find("/flow/serial_1/message_1")->is_active());
      CHECK(!fp->find("/flow/serial_1")->is_active());
      CHECK(fp->find("/flow/message_top_1")->is_active());
      fp->update();  // next
      CHECK(fp->find("/flow/message_top_2")->is_active());
      fp->update();  // serial_1
      CHECK(fp->find("/flow/serial_1")->is_active());
      CHECK(fp->find("/flow/serial_1/message_1")->is_active());
    }
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
}

TEST_CASE("ensure - bot")
{
  SUBCASE("bot start and run timers")
  {
    std::string conf = R"(
    {
    "ensure" : {
      "concurrency" : 1, 
      "port" : 7000, 
      "bot_count" : 1, 
      "bot_start_index" : 1, 
      "bot_prefix" : "bot_"
    },
    "flow": [
      {
      "type" : "act_message", 
      "name" : "message_top_1",
      "message" : "hello ensure! I'm called from the strand timer",
      "slots" : { 
        "success" : { "cmd" : "next" }, 
        "fail" : { "cmd" : "next" }
        }
      },
      {
      "type" : "act_message", 
      "name" : "message_top_2",
      "message" : "hello ensure! Do I have a chance to be called?",
      "slots" : { 
        "success" : { "cmd" : "exit" },
        "fail" : { "cmd" : "exit" }
        }
      }
    ]
    }
    )";

    ensure& app = ensure::get();  // ensure app은 싱글톤
    auto jconf = nlohmann::json::parse(conf);
    CHECK(app.start(jconf));
    CHECK(app.get_bot_count() == 1);
    auto top_1 = app.get_bot(0)->get_flow().find("/flow/message_top_1");
    CHECK(top_1->get_name() == "message_top_1");
    CHECK(top_1->is_active());  // XXX: not sure. how to make it sure?

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    app.stop();
  }
}