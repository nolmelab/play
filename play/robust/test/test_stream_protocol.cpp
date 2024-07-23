#include <doctest/doctest.h>
#include <chrono>
#include <play/robust/net/client.hpp>
#include <play/robust/net/protocol/stream_protocol.hpp>
#include <play/robust/net/runner/poll_runner.hpp>
#include <play/robust/net/server.hpp>

using namespace play::robust::net;

TEST_CASE("client & server")
{
  SUBCASE("compile server")
  {
    poll_runner runner;
    server<stream_protocol> server(runner);
  }
}

namespace {

static constexpr int test_bytes = 1024;

struct test_server : public server<stream_protocol>
{
  using server = server<stream_protocol>;

  test_server(runner& runner)
      : server(runner)
  {
  }

  void handle_established(session_ptr session) final {}

  void handle_closed(session_ptr session, error_code ec) final {}

  void handle_receive(session_ptr session, topic topic, const void* data, size_t len) final
  {
    recv_bytes_ += len;
    session->send(reinterpret_cast<const char*>(data), len);  // echo back
    // LOG()->info("server. byets: {}", recv_bytes_);
  }

  size_t recv_bytes_{0};
};

struct test_client : public client<stream_protocol>
{
  using client = client<stream_protocol>;

  test_client(runner& runner)
      : client(runner)
  {
  }

  void send(const char* data, size_t len)
  {
    get_session()->send(reinterpret_cast<const char*>(data), len);  // echo back
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
    session->send(reinterpret_cast<const char*>(data), len);  // echo back
    // LOG()->info("client. byets: {}", recv_bytes_);
  }

  size_t recv_bytes_{0};
};

}  // namespace

TEST_CASE("communication")
{
  LOG()->set_level(spdlog::level::trace);

  SUBCASE("stream_protocol")
  {
    poll_runner runner{"stream_protocol runner"};
    test_server server(runner);
    test_client client(runner);

    auto rc = server.start(7000);

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
  }

  SUBCASE("stream_protocol echo perf")
  {
    poll_runner runner{"stream_protocol runner"};
    test_server server(runner);
    test_client client(runner);

    auto rc = server.start(7000);

    client.connect("127.0.0.1", 7000);
    runner.poll_one();

    auto start = std::chrono::steady_clock::now();

    while (client.recv_bytes_ < test_bytes)
    {
      if (runner.poll_one() == 0)
      {
        runner.sleep(1);
      }
    }

    server.stop();

    auto end = std::chrono::steady_clock::now();

    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    LOG()->info("stream recv: {} bytes. elapsed: {}", test_bytes, elapsed);
  }
}