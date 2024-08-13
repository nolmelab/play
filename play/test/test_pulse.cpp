#include <doctest/doctest.h>
#include <play/test/test_fb_generated.h>
#include <play/base/stop_watch.hpp>
#include <play/net/client.hpp>
#include <play/net/protocol/secure_protocol.hpp>
#include <play/net/pulse_fb.hpp>
#include <play/net/runner/poll_runner.hpp>
#include <play/net/server.hpp>

using namespace play;

namespace {

// 1, 2는 시스템에서 사용
const uint16_t topic_message = 11;

struct test_config
{
  inline static int test_frame_count = 1024;
  inline static int payload_loops = 100;
  inline static std::string payload_data = "hello";
  inline static bool client_encrypt = true;
  inline static bool server_encrypt = false;
  inline static int echo_count = 5;
};

}  // namespace

// test_flatbuffers와 동일한 테스트를 pulse로 진행한다.
/**
 * pulse_fb 내에 server와 client를 실행한다. 
 * pulse에 구독한 핸들러 람다에서 에코를 처리한다. 
 * topic_established에서 전송을 시작한다. 
 */
TEST_CASE("pulse")
{
  using pulse_t = pulse_fb<secure_protocol<uint16_t>>;

  size_t recv_count = 0;
  play::stop_watch watch;

  poll_runner runner{"pulse runner"};
  pulse_t pulse_server;
  pulse_t pulse_client;

  pulse_t::add_unpacker(topic_message, &pulse_t::unpack_fn<fb::req_move, fb::req_moveT>);

  pulse_server.as_server(&runner, 7000)
      .sub(topic_message,
           [](pulse_t::session_ptr se, pulse_t::frame_ptr fr)
           {
             auto req_move = std::static_pointer_cast<fb::req_moveT>(fr);
             auto v = req_move->pos->x();
             pulse_t::send<fb::req_move>(se, topic_message, *req_move.get(), false);
           });

  pulse_client.as_client(&runner, "127.0.0.1:7000")
      .sub(pulse_t::topic_estalished,
           [](pulse_t::session_ptr se, pulse_t::frame_ptr fr)
           {
             fb::req_moveT move;
             move.pos = std::make_unique<fb::vec3>(1, 1, 1);
             move.name = "hello";

             pulse_t::send<fb::req_move>(se, topic_message, move, false);
           })
      .sub(topic_message,
           [&recv_count](pulse_t::session_ptr se, pulse_t::frame_ptr fr)
           {
             recv_count++;
             auto req_move = std::static_pointer_cast<fb::req_moveT>(fr);
             auto v = req_move->pos->x();
             pulse_t::send<fb::req_move>(se, topic_message, *req_move.get(), true);
           });

  CHECK(pulse_server.start());
  CHECK(pulse_client.start());

  runner.poll_one();

  bool end = false;

  while (recv_count < test_config::test_frame_count)
  {
    if (runner.poll_one() == 0)
    {
      runner.sleep(1);
    }
  }

  pulse_server.stop();
  pulse_client.stop();

  auto elapsed = watch.stop();

  LOG()->info("pulse elapsed: {}, frames; {}", elapsed, test_config::test_frame_count);
}

TEST_CASE("pulse interests")
{
  using pulse = pulse_fb<secure_protocol<uint16_t>>;

  poll_runner runner{"pulse runner"};

  SUBCASE("uintptr_t")
  {
    auto sp_1 = std::make_shared<int>(3);
    auto sp_2 = std::shared_ptr<int>{};
    auto key_1 = reinterpret_cast<uintptr_t>(sp_1.get());
    auto key_2 = reinterpret_cast<uintptr_t>(sp_2.get());

    LOG()->info("key_1: {}, key_2: {}, size: {}", key_1, key_2, sizeof(uintptr_t));
  }

  SUBCASE("pulse chain")
  {

    SUBCASE("a root and a child")
    {
      pulse root;
      root.as_independent(&runner).start();

      size_t call_count = 0;

      pulse p1;
      p1.as_child(&root)
          .subscribe(11,
                     [&call_count](pulse::session_ptr se, pulse::frame_ptr fr)
                     {
                       call_count++;
                     })
          .start();

      auto move = std::make_shared<fb::req_moveT>();
      move->pos = std::make_unique<fb::vec3>(1, 1, 1);
      move->name = "hello";

      std::shared_ptr<pulse::session> se;
      root.publish(se, 11, move);
      runner.poll_one();
      CHECK(call_count == 1);

      p1.stop();
      root.publish(se, 11, move);
      runner.poll_one();
      CHECK(call_count == 1);
    }

    SUBCASE("a root and children")
    {
      pulse root;
      root.as_independent(&runner).start();

      size_t call_count = 0;

      pulse p1;
      p1.as_child(&root)
          .subscribe(11,
                     [&call_count](pulse::session_ptr se, pulse::frame_ptr fr)
                     {
                       call_count++;
                     })
          .start();

      pulse p2;
      p2.as_child(&root)
          .subscribe(11,
                     [&call_count](pulse::session_ptr se, pulse::frame_ptr fr)
                     {
                       call_count++;
                     })
          .start();

      auto move = std::make_shared<fb::req_moveT>();
      move->pos = std::make_unique<fb::vec3>(1, 1, 1);
      move->name = "hello";

      std::shared_ptr<pulse::session> se;
      root.publish(se, 11, move);
      runner.poll_one();
      runner.poll_one();
      runner.poll_one();
      CHECK(call_count == 2);

      p1.stop();
      root.publish(se, 11, move);
      runner.poll_one();
      runner.poll_one();
      CHECK(call_count == 3);

      p2.stop();
      root.publish(se, 11, move);
      runner.poll_one();
      CHECK(call_count == 3);
    }
  }
}

namespace {
class mockup_session_handler : public session_handler<session<secure_protocol<uint16_t>>>
{
public:
  void on_established(session_ptr se) final {}

  void on_closed(session_ptr se, error_code ec) final {}

  void on_receive(session_ptr se, topic topic, const void* data, size_t len) final {}
};
}  // namespace

TEST_CASE("with_session")
{
  using pulse = pulse_fb<secure_protocol<uint16_t>>;
  poll_runner runner{"pulse runner"};

  SUBCASE("session filtering")
  {
    //
    pulse root;
    root.as_independent(&runner).start();

    size_t call_count = 0;

    mockup_session_handler handler;
    boost::asio::io_context ioc;

    auto se_1 = std::make_shared<pulse::session>(handler, ioc, false);
    auto se_2 = std::make_shared<pulse::session>(handler, ioc, false);

    pulse p1;
    p1.as_child(&root)
        .with_session(se_1)
        .subscribe(11,
                   [&call_count](pulse::session_ptr se, pulse::frame_ptr fr)
                   {
                     call_count++;
                   })
        .start();

    pulse p2;
    p2.as_child(&root)
        .with_session(se_2)
        .subscribe(11,
                   [&call_count](pulse::session_ptr se, pulse::frame_ptr fr)
                   {
                     call_count++;
                   })
        .start();

    auto move = std::make_shared<fb::req_moveT>();
    move->pos = std::make_unique<fb::vec3>(1, 1, 1);
    move->name = "hello";

    root.publish(se_1, 11, move);
    runner.poll_one();
    runner.poll_one();
    CHECK(call_count == 1);

    root.publish(se_2, 11, move);
    runner.poll_one();
    runner.poll_one();
    CHECK(call_count == 2);

    p1.stop();
    p2.stop();

    root.publish(se_1, 11, move);
    runner.poll_one();
    runner.poll_one();
    CHECK(call_count == 2);
  }

  SUBCASE("session blind subscription")
  {
    //
    pulse root;
    root.as_independent(&runner).start();

    size_t call_count = 0;

    mockup_session_handler handler;
    boost::asio::io_context ioc;

    auto se_1 = std::make_shared<pulse::session>(handler, ioc, false);
    auto se_2 = std::make_shared<pulse::session>(handler, ioc, false);

    pulse p1;
    p1.as_child(&root)
        .subscribe(11,
                   [&call_count](pulse::session_ptr se, pulse::frame_ptr fr)
                   {
                     call_count++;
                   })
        .with_session(se_1)
        .subscribe(11,
                   [&call_count](pulse::session_ptr se, pulse::frame_ptr fr)
                   {
                     call_count++;
                   })
        .start();

    pulse p2;
    p2.as_child(&root)
        .with_session(se_2)
        .subscribe(11,
                   [&call_count](pulse::session_ptr se, pulse::frame_ptr fr)
                   {
                     call_count++;
                   })
        .start();

    auto move = std::make_shared<fb::req_moveT>();
    move->pos = std::make_unique<fb::vec3>(1, 1, 1);
    move->name = "hello";

    root.publish(se_1, 11, move);
    runner.poll_one();
    runner.poll_one();
    CHECK(call_count == 2);

    root.publish(se_2, 11, move);
    runner.poll_one();
    runner.poll_one();
    CHECK(call_count == 4);

    p1.stop();
    p2.stop();

    root.publish(se_1, 11, move);
    runner.poll_one();
    runner.poll_one();
    CHECK(call_count == 4);
  }
}

TEST_CASE("pulse call")
{
  using pulse = pulse_fb<secure_protocol<uint16_t>>;
  const size_t topic_req = 11;
  const size_t topic_res = 12;

  pulse::add_unpacker(topic_req, &pulse::unpack_fn<fb::req_move, fb::req_moveT>);
  pulse::add_unpacker(topic_res, &pulse::unpack_fn<fb::res_move, fb::res_moveT>);

  size_t recv_count = 0;

  poll_runner runner{"pulse runner"};
  pulse pulse_server;
  pulse pulse_client;

  pulse pulse_child;
  pulse_child.as_child(&pulse_client);

  auto result_1 = pulse_server.as_server(&runner, 8000)
                      .subscribe(topic_req,
                                 [](pulse::session_ptr se, pulse::frame_ptr fr)
                                 {
                                   fb::res_moveT res_move;
                                   res_move.pos = std::make_unique<fb::vec3>(1, 1, 1);
                                   pulse::send<fb::res_move>(se, topic_res, res_move);
                                 })
                      .start();

  auto result_2 =
      pulse_client.as_client(&runner, "127.0.0.1:8000")
          .subscribe(pulse::topic_estalished,
                     [&pulse_child](pulse::session_ptr se, pulse::frame_ptr fr)
                     {
                       pulse_child.with_session(se).start();
                       fb::req_moveT req_move;
                       req_move.pos = std::make_unique<fb::vec3>(1, 1, 1);
                       req_move.name = "hello call";
                       pulse_child.call<fb::req_move>(se, topic_req, topic_res, req_move,
                                                      []()
                                                      {
                                                        LOG()->error("call failed");
                                                      });
                     })
          .subscribe(topic_res,
                     [&recv_count, &pulse_child](pulse::session_ptr se, pulse::frame_ptr fr)
                     {
                       ++recv_count;
                       fb::req_moveT req_move;
                       req_move.pos = std::make_unique<fb::vec3>(1, 1, 1);
                       req_move.name = "hello call";
                       pulse_child.call<fb::req_move>(se, topic_req, topic_res, req_move,
                                                      []()
                                                      {
                                                        LOG()->error("call failed");
                                                      });
                     })
          .start();

  CHECK(result_1);
  CHECK(result_2);

  runner.poll_one();

  bool end = false;

  while (recv_count < test_config::test_frame_count)
  {
    if (runner.poll_one() == 0)
    {
      runner.sleep(1);
    }
  }

  pulse_server.stop();
  pulse_client.stop();
}

TEST_CASE("std::map perf")
{

  size_t test_count = 10;

  std::map<int, int> m;

  for (size_t i = 0; i < test_count; ++i)
  {
    m.insert(std::pair{i, i});
  }

  play::stop_watch watch;

  for (size_t i = 0; i < test_count; ++i)
  {
    m.find(i);
  }

  LOG()->info("map elapsed: {}", watch.get_elapsed());

  // session, topic 기반 관심 관리시 구독이 많아지면 수백만 건이 될 수 있어 성능을 측정.
  // - 1억건, 릴리스 0초. find는 부하가 거의 없다. nano초 미만이다.
  // insert에서 메모리 할당과 트리 균형 작업으로 느리다.
  // 문제가 되지는 않으나 불편해 하는 사람들이 있을 수 있다. 더 나은(?) 방법이
  // 있다고 생각할 수 있기 때문이다.
}