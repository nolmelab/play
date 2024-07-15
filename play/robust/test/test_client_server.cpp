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

  test_server(runner& runner, std::string_view json)
      : server(runner, json)
  {
  }

  void handle_established(session_ptr session) final {}

  void handle_closed(session_ptr session, boost::system::error_code ec) final {}

  void handle_receive(session_ptr session, topic topic, const void* data, size_t len) final
  {
    recv_bytes_ += len;
    session->get_protocol().send(reinterpret_cast<const char*>(data), len);  // echo back
  }

  size_t recv_bytes_;
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
    get_session()->get_protocol().send(reinterpret_cast<const char*>(data), len);  // echo back
  }

  void handle_established(session_ptr session) final
  {
    LOG()->info("test_client established. remote: {}", session->get_remote_addr());
    send("hello", 5);
  }

  void handle_closed(session_ptr session, boost::system::error_code ec) final {}

  void handle_receive(session_ptr session, topic topic, const void* data, size_t len) final
  {
    recv_bytes_ += len;
    session->get_protocol().send(reinterpret_cast<const char*>(data), len);  // echo back
  }

  size_t recv_bytes_{0};
};

}  // namespace

TEST_CASE("communication")
{
  SUBCASE("stream_protocol")
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
    runner.poll();
    runner.poll();

    bool end = false;

    while (client.recv_bytes_ < 1024)
    {
      if (runner.poll() == 0)
      {
        runner.sleep(1);
      }
    }

    server.stop();

    // [1] excpetion 처리 추가 : server와 client 모두 필요한 곳에 추가
    // [2] connect / accept 후 통신이 진행되지 않음
    //     - established가 호출되나 send()가 동작하지 않음
    //     - stream_protocol::accepted()에서 호출 하지 않는 버그 OK
    //     - 이후에도 통신이 안 됨
    //       - socket_.async_receive()로 해야 일부만 받음. asio::async_read()는 버퍼 크기만큼 받음
    //       - 이제 잘 됨
    // [3] native_handle 소켓에서 얻은 값이 이상하여 start()에서 지정하도록 변경
    //     - 제대로 보임
    // [4] reuse address 옵션 추가. 디버깅할 때 매우 불편
    //     - 서비스 할 때는 TIME_WAIT 값 조절로 해결해야 함
    //     - 포트를 훔칠 수 있으므로 옵션으로 하는 것 고려
    // [5] 통신 되고 나서 poll()에서 리턴되지 않고 계속 실행됨
    //     -
  }
}