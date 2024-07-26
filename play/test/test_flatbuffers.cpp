#include <doctest/doctest.h>
#include <play/test/test_fb_generated.h>
#include <play/base/stop_watch.hpp>
#include <play/net/client.hpp>
#include <play/net/frame/flatbuffer_handler.hpp>
#include <play/net/protocol/secure_protocol.hpp>
#include <play/net/runner/poll_runner.hpp>
#include <play/net/server.hpp>

using namespace play;

namespace {

struct test_config
{
  inline static int test_frame_count = 1024;
  inline static int payload_loops = 100;
  inline static std::string payload_data = "hello";
  inline static bool client_encrypt = true;
  inline static bool server_encrypt = false;
  inline static int echo_count = 5;
};

using server_t = server<secure_protocol<uint16_t>, flatbuffers::NativeTable>;
using client_t = client<secure_protocol<uint16_t>, flatbuffers::NativeTable>;

struct test_server : public server_t
{
  test_server(runner& runner, server_t::frame_handler& handler)
      : server_t(runner, handler)
  {
  }
};

struct test_client : public client_t
{
  test_client(runner& runner, client_t::frame_handler& handler)
      : client_t(runner, handler)
  {
  }

  void handle_established(session_ptr session) final
  {
    LOG()->info("test_client established. remote: {}", session->get_remote_addr());

    fb::req_moveT move;
    move.pos = std::make_unique<fb::vec3>(1, 1, 1);
    move.name = "hello";

    get_handler<flatbuffer_handler<test_client::session>>().send<fb::req_move>(session, 1, move,
                                                                               false);
  }

  size_t recv_count_{0};
};

}  // namespace

TEST_CASE("faltbuffers")
{
  using server_handler_t = flatbuffer_handler<test_server::session>;
  using client_handler_t = flatbuffer_handler<test_client::session>;

  server_handler_t server_handler;
  client_handler_t client_handler;

  server_handler.reg(1, &server_handler_t::unpack<fb::req_move, fb::req_moveT>);
  client_handler.reg(1, &server_handler_t::unpack<fb::req_move, fb::req_moveT>);

  poll_runner runner{"secure_protocol runner"};
  test_server server(runner, server_handler);
  test_client client(runner, client_handler);

  server_handler_t::receiver cb =
      [&server_handler](server_handler_t::session_ptr se, server_handler_t::frame_ptr f)
  {
    auto req_move = std::static_pointer_cast<fb::req_moveT>(f);
    auto v = req_move->pos->x();
    server_handler.send<fb::req_move>(se, 1, *req_move.get(), false);
  };
  server_handler.sub(1, cb);

  client_handler_t::receiver cb_2 =
      [&client, &client_handler](client_handler_t::session_ptr se, client_handler_t::frame_ptr f)
  {
    client.recv_count_++;
    auto req_move = std::static_pointer_cast<fb::req_moveT>(f);
    auto v = req_move->pos->x();
    client_handler.send<fb::req_move>(se, 1, *req_move.get(), true);
  };
  client_handler.sub(1, cb_2);

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

TEST_CASE("flatbuffer_handler")
{
  SUBCASE("sub / unsub")
  {
    using server_handler_t = flatbuffer_handler<test_server::session>;

    server_handler_t h1;
    server_handler_t::receiver cb =
        [](server_handler_t::session_ptr se, server_handler_t::frame_ptr f)
    {
    };
    auto sub_id_1 = h1.sub(1, cb);
    auto sub_id_2 = h1.sub(1, cb);
    CHECK(h1.get_subs_size(1) == 2);
    h1.unsub(1, sub_id_1);
    h1.unsub(1, sub_id_2);
    CHECK(h1.get_subs_size(1) == 0);
  }

  SUBCASE("sub / unsub_topic")
  {
    using server_handler_t = flatbuffer_handler<test_server::session>;

    server_handler_t h1;
    server_handler_t::receiver cb =
        [](server_handler_t::session_ptr se, server_handler_t::frame_ptr f)
    {
    };
    auto sub_id_1 = h1.sub(1, cb);
    auto sub_id_2 = h1.sub(1, cb);
    CHECK(h1.get_subs_size(1) == 2);
    h1.unsub_topic(1);
    CHECK(h1.get_subs_size(1) == 0);
  }
}