#include <doctest/doctest.h>
#include <play/robust/base/stop_watch.hpp>
#include <play/robust/net/client.hpp>
#include <play/robust/net/flatbuffers/flatbuffer_handler.hpp>
#include <play/robust/net/protocol/secure_protocol.hpp>
#include <play/robust/net/runner/poll_runner.hpp>
#include <play/robust/net/server.hpp>

using namespace play::robust::net;

namespace {

struct test_config
{
  inline static int test_bytes = 1024;
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
  test_server(runner& runner, std::string_view json, server_t::frame_handler& handler)
      : server_t(runner, json, handler)
  {
  }
};

struct test_client : public client_t
{
  test_client(runner& runner, client_t::frame_handler& handler)
      : client_t(runner, handler)
  {
  }

  size_t recv_bytes_{0};
  std::string payload_;
};

}  // namespace

TEST_CASE("faltbuffers")
{
  flatbuffer_handler<test_server::session> server_handler;
  flatbuffer_handler<test_client::session> client_handler;

  //
  poll_runner runner{"secure_protocol runner"};

  test_server server(runner, R"(
    {
      "port" : 7000, 
      "concurrency" : 8
    })",
                     server_handler);

  test_client client(runner, client_handler);

  auto rc = server.start();

  play::robust::base::stop_watch watch;

  client.connect("127.0.0.1", 7000);
  runner.poll_one();

  bool end = false;

  while (client.recv_bytes_ < test_config::test_bytes)
  {
    if (runner.poll_one() == 0)
    {
      runner.sleep(1);
    }
  }

  server.stop();

  auto elapsed = watch.stop();

  LOG()->info("elapsed: {}, bytes; {}", elapsed, test_config::test_bytes);
}