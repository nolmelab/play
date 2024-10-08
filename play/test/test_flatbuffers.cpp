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

using server_t = server<secure_protocol<uint16_t>>;
using client_t = client<secure_protocol<uint16_t>>;
using pulse_t = pulse_fb<secure_protocol<uint16_t>>;

struct test_server : public server_t
{
  test_server(runner& runner)
      : server_t(runner)
  {
  }
};

struct test_client : public client_t
{
  test_client(runner& runner)
      : client_t(runner)
  {
  }

  void handle_established(session_ptr session) final
  {
    LOG()->info("test_client established. remote: {}", session->get_remote_addr());

    fb::req_moveT move;
    move.pos = std::make_unique<fb::vec3>(1, 1, 1);
    move.name = "hello";

    pulse_t::send<fb::req_move>(session, topic_message, move, false);
  }

  size_t recv_count_{0};
};

}  // namespace

TEST_CASE("flatbuffers")
{
  poll_runner runner{"secure_protocol runner"};

  pulse_t pulse_server;
  pulse_t pulse_client;

  pulse_server.as_independent(&runner);
  pulse_client.as_independent(&runner);

  pulse_t::add_unpacker(topic_message, &pulse_t::unpack_fn<fb::req_move, fb::req_moveT>);

  test_server server(runner);
  test_client client(runner);

  server.bind_pulse(&pulse_server);
  client.bind_pulse(&pulse_client);

  pulse_t::receiver cb_1 = [](pulse_t::session_ptr se, pulse_t::frame_ptr f)
  {
    auto req_move = std::static_pointer_cast<fb::req_moveT>(f);
    auto v = req_move->pos->x();
    pulse_t::send<fb::req_move>(se, topic_message, *req_move.get(), false);
  };

  pulse_server.subscribe(topic_message, cb_1);

  pulse_t::receiver cb_2 = [&client](pulse_t::session_ptr se, pulse_t::frame_ptr f)
  {
    client.recv_count_++;
    auto req_move = std::static_pointer_cast<fb::req_moveT>(f);
    auto v = req_move->pos->x();
    pulse_t::send<fb::req_move>(se, topic_message, *req_move.get(), true);
  };

  pulse_client.subscribe(topic_message, cb_2);

  auto rc = server.start(7000);

  play::stop_watch watch;

  client.connect("127.0.0.1", 7000);
  runner.poll_one();

  bool end = false;

  while (client.recv_count_ < test_config::test_frame_count)
  {
    if (runner.poll_one() == 0)
    {
      runner.sleep(1);
    }
  }

  server.stop();

  auto elapsed = watch.stop();

  LOG()->info("elapsed: {}, frames; {}", elapsed, test_config::test_frame_count);
}
