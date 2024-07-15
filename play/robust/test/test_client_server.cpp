#include <doctest/doctest.h>
#include <play/robust/net/client.hpp>
#include <play/robust/net/protocol/plain_protocol.hpp>
#include <play/robust/net/protocol/stream_protocol.hpp>
#include <play/robust/net/runner/poll_runner.hpp>
#include <play/robust/net/server.hpp>

using namespace play::robust::net;

TEST_CASE("client & server")
{
  SUBCASE("compile server")
  {
    poll_runner runner;
    server<plain_protocol<uint32_t>> server(runner, R"(
    {
      "port" : 7000, 
      "concurrency" : 8
    })");

    // 컴파일이 되었다.
    // - dependent name lookup 관련 오류 => this->shared_from_this()로 해결
    // - std::make_shared<session<Protocol>>()이 안 되는 오류
    //   => using으로 session을 정의하고 해결
    // - adapter를 임의로 제공하고 싶었으나 수월하지 않다.
    //   - 최적은 무엇일까? 함수 포인터로 만든다. 그러면 가상 함수도 필요없다.
    //   - C의 접근이 최적일 때가 있다. 돌아서 여기에 왔다.
  }
}

namespace {

struct test_server : public server<stream_protocol>
{
  using server = server<stream_protocol>;

  test_server(runner& runner, std::string_view json) : server(runner, json) {}

  void handle_established(session_ptr session) final {}

  void handle_closed(session_ptr session, boost::system::error_code ec) final {}

  void handle_receive(session_ptr session, topic topic, const void* data, size_t len) final
  {
    recv_bytes_ += len;
  }

  size_t recv_bytes_;
};

struct test_client : public client<stream_protocol>
{
  using client = client<stream_protocol>;

  test_client(runner& runner) : client(runner) {}

  void handle_established(session_ptr session) final {}

  void handle_closed(session_ptr session, boost::system::error_code ec) final {}

  void handle_receive(session_ptr session, topic topic, const void* data, size_t len) final
  {
    recv_bytes_ += len;
  }

  size_t recv_bytes_{0};
};

}  // namespace

TEST_CASE("communication")
{
  SUBCASE("stream_protocol")
  {
    poll_runner runner;

    server<stream_protocol> server(runner, R"(
    {
      "port" : 7000, 
      "concurrency" : 8
    })");

    client<stream_protocol> client(runner);

    auto rc = server.start();

    client.connect("127.0.0.1", 7000);

    bool end = false;

    while (!end)
    {
      if (runner.poll() == 0)
      {
        runner.sleep(1);
      }
    }
  }
}