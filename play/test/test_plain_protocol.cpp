#include <doctest/doctest.h>
#include <play/base/stop_watch.hpp>
#include <play/net/client.hpp>
#include <play/net/protocol/plain_protocol.hpp>
#include <play/net/runner/poll_runner.hpp>
#include <play/net/server.hpp>

using namespace play;

namespace {

static constexpr int test_bytes = 1 * 1024;

struct test_server : public server<plain_protocol<uint32_t>>
{
  test_server(frame_handler& handler)
      : server<plain_protocol<uint32_t>>(handler)
  {
  }

  void handle_established(session_ptr session) final {}

  void handle_closed(session_ptr session, error_code ec) final {}

  void handle_receive(session_ptr session, topic topic, const void* data, size_t len) final
  {
    recv_bytes_ += len;
    session->send(1, reinterpret_cast<const char*>(data), len);  // echo back
  }

  size_t recv_bytes_{0};
};

struct test_client : public client<plain_protocol<uint32_t>>
{
  test_client(frame_handler& handler)
      : client<plain_protocol<uint32_t>>(handler)
  {
  }

  void send(const char* data, size_t len)
  {
    get_session()->send(1, reinterpret_cast<const char*>(data), len);  // echo back
  }

  void handle_established(session_ptr session) final
  {
    LOG()->info("test_client established. remote: {}", session->get_remote_addr());
    std::string payload;
    for (int i = 0; i < 100; ++i)
    {
      payload.append("hello");
    }
    send(payload.c_str(), payload.length());
  }

  void handle_closed(session_ptr session, error_code ec) final {}

  void handle_receive(session_ptr session, topic topic, const void* data, size_t len) final
  {
    recv_bytes_ += len;
    session->send(1, reinterpret_cast<const char*>(data), len);  // echo back
  }

  size_t recv_bytes_{0};
};

}  // namespace

TEST_CASE("plain_protocol")
{
  SUBCASE("basics")
  {
    poll_runner runner{"plain_protocol runner"};
    frame_default_handler<test_server> handler{runner};
    test_server server(handler);
    test_client client(handler);

    auto rc = server.start(7000);

    play::stop_watch watch;

    client.connect("127.0.0.1", 7000);
    runner.poll_one();

    bool end = false;

    while (client.recv_bytes_ < test_bytes)
    {
      if (runner.poll_one() == 0)
      {
        runner.sleep(1);
      }
    }

    server.stop();

    auto elapsed = watch.stop();

    LOG()->info("plain_protocol. elapsed: {}, bytes; {}", elapsed, test_bytes);
  }
}