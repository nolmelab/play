#include <doctest/doctest.h>
#include <play/robust/base/stop_watch.hpp>
#include <play/robust/net/client.hpp>
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

struct test_server : public server<secure_protocol<uint32_t>>
{
  test_server(runner& runner)
      : server<secure_protocol<uint32_t>>(runner)
  {
  }

  void handle_established(session_ptr session) final
  {
    LOG()->info("test_server established. remote: {}", session->get_remote_addr());
  }

  void handle_closed(session_ptr session, error_code ec) final {}

  void handle_receive(session_ptr session, uint32_t topic, const void* data, size_t len) final
  {
    recv_bytes_ += len;
    for (int i = 0; i < test_config::echo_count; ++i)
    {
      session->send(1, reinterpret_cast<const char*>(data), len,
                    test_config::server_encrypt);  // echo back
    }
  }

  size_t recv_bytes_{0};
};

struct test_client : public client<secure_protocol<uint32_t>>
{
  test_client(runner& runner)
      : client<secure_protocol<uint32_t>>(runner)
  {
  }

  void send(const char* data, size_t len)
  {
    get_session()->send(1, reinterpret_cast<const char*>(data), len, true);  // echo back
  }

  void handle_established(session_ptr session) final
  {
    LOG()->info("test_client established. remote: {}", session->get_remote_addr());

    for (int i = 0; i < test_config::payload_loops; ++i)
    {
      payload_.append(test_config::payload_data);
    }
    send(payload_.c_str(), payload_.length());
  }

  void handle_closed(session_ptr session, error_code ec) final {}

  void handle_receive(session_ptr session, uint32_t topic, const void* data, size_t len) final
  {
    recv_bytes_ += len;
    std::string rs{reinterpret_cast<const char*>(data), len};
    CHECK(rs == payload_);

    for (int i = 0; i < test_config::echo_count; ++i)
    {
      session->send(1, reinterpret_cast<const char*>(data), len, test_config::client_encrypt);
    }
  }

  size_t recv_bytes_{0};
  std::string payload_;
};

void run_test(std::string_view name)
{
  poll_runner runner{"secure_protocol runner"};
  test_server server(runner);
  test_client client(runner);

  auto rc = server.start(7000);

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

  LOG()->info("{}. elapsed: {}, bytes; {}", name, elapsed, test_config::test_bytes);
}

}  // namespace

TEST_CASE("secure_protocol")
{
  SUBCASE("basics")
  {
    run_test("basics");
  }

  SUBCASE("1 byte payload")
  {
    test_config::payload_data = "1";
    test_config::payload_loops = 1;

    run_test("small payload");
  }

  SUBCASE("big payload")
  {
    test_config::payload_data = "big_";
    test_config::payload_loops = 100000;

    run_test("big payload");
  }
}