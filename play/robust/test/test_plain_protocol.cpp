#include <doctest/doctest.h>
#include <play/robust/net/client.hpp>
#include <play/robust/net/protocol/plain_protocol.hpp>
#include <play/robust/net/runner/poll_runner.hpp>
#include <play/robust/net/server.hpp>

using namespace play::robust::net;

namespace {

static constexpr int test_bytes = 1 * 1024 * 1024;

struct test_server : public server<plain_protocol<uint32_t>>
{
  test_server(runner& runner, std::string_view json)
      : server<plain_protocol<uint32_t>>(runner, json)
  {
  }

  void handle_established(session_ptr session) final {}

  void handle_closed(session_ptr session, boost::system::error_code ec) final {}

  void handle_receive(session_ptr session, topic topic, const void* data, size_t len) final
  {
    recv_bytes_ += len;
    session->get_protocol().send(1, reinterpret_cast<const char*>(data), len);  // echo back
  }

  size_t recv_bytes_{0};
};

struct test_client : public client<plain_protocol<uint32_t>>
{
  test_client(runner& runner)
      : client<plain_protocol<uint32_t>>(runner)
  {
  }

  void send(const char* data, size_t len)
  {
    get_session()->get_protocol().send(1, reinterpret_cast<const char*>(data), len);  // echo back
  }

  void handle_established(session_ptr session) final
  {
    LOG()->info("test_client established. remote: {}", session->get_remote_addr());
    std::string payload;
    for (int i; i < 100; ++i)
    {
      payload.append("hello");
    }
    send(payload.c_str(), payload.length());
  }

  void handle_closed(session_ptr session, boost::system::error_code ec) final {}

  void handle_receive(session_ptr session, topic topic, const void* data, size_t len) final
  {
    recv_bytes_ += len;
    session->get_protocol().send(1, reinterpret_cast<const char*>(data), len);  // echo back
  }

  size_t recv_bytes_{0};
};

}  // namespace

TEST_CASE("plain_protocol")
{
  SUBCASE("basics")
  {
    poll_runner runner;

    test_server server(runner, R"(
    {
      "port" : 7000, 
      "concurrency" : 8
    })");

    test_client client(runner);

    auto rc = server.start();

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

    // [1] send() 호출이 진행되었으나 통신이 되지 않는다.
    // - 길이가 503으로 오류가 있다.
    // - topic 처리를 consume 하면 안 된다. unconsume 기능이 없기 때문이다.
    // - length codec의 인터페이스가 달라져야 한다.

    // [2] plain_protocol에서 전송시 불필요한 복사가 한 번 발생한다.
    // - 헤더만 보내고, 그 다음 나머지를 보낸다.
    // - adapter에 두 개 퍼버를 받는 인터페이스가 필요하다.

    // [3] plain_protocol에서 수신시 불필요한 복사가 한 번 발생한다.
    // - streambuf를 받아서 처리하도록 한다.
    // - 전체 인터페이스의 수정이 필요하다.
  }
}