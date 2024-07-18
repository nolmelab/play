#include <doctest/doctest.h>
#include <play/robust/base/stop_watch.hpp>
#include <play/robust/net/client.hpp>
#include <play/robust/net/protocol/secure_protocol.hpp>
#include <play/robust/net/runner/poll_runner.hpp>
#include <play/robust/net/server.hpp>

using namespace play::robust::net;

namespace {

static constexpr int test_bytes = 1024;

struct test_server : public server<secure_protocol<uint32_t>>
{
  test_server(runner& runner, std::string_view json)
      : server<secure_protocol<uint32_t>>(runner, json)
  {
  }

  void handle_established(session_ptr session) final
  {
    LOG()->info("test_server established. remote: {}", session->get_remote_addr());
  }

  void handle_closed(session_ptr session, boost::system::error_code ec) final {}

  void handle_receive(session_ptr session, uint32_t topic, const void* data, size_t len) final
  {
    recv_bytes_ += len;
    session->send(1, reinterpret_cast<const char*>(data), len, true);  // echo back
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
    //for (int i = 0; i < 100; ++i)
    {
      // payload_.append(fmt::format("[{} hello]", i));
      payload_.append("1");  // 1바이트 송수신
    }
    send(payload_.c_str(), payload_.length());
  }

  void handle_closed(session_ptr session, boost::system::error_code ec) final {}

  void handle_receive(session_ptr session, uint32_t topic, const void* data, size_t len) final
  {
    recv_bytes_ += len;
    session->send(1, reinterpret_cast<const char*>(data), len);  // echo back
    session->send(1, reinterpret_cast<const char*>(data), len);  // echo back

    std::string rs{reinterpret_cast<const char*>(data), len};
    CHECK(rs == payload_);
  }

  size_t recv_bytes_{0};
  std::string payload_;
};

}  // namespace

TEST_CASE("secure_protocol")
{
  SUBCASE("basics")
  {
    poll_runner runner{"secure_protocol runner"};

    test_server server(runner, R"(
    {
      "port" : 7000, 
      "concurrency" : 8
    })");

    test_client client(runner);

    auto rc = server.start();

    play::robust::base::stop_watch watch;

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

    LOG()->info("secure_protocol. elapsed: {}, bytes; {}", elapsed, test_bytes);
  }

  // 테스트들:
  // - 1 바이트 두번 전송으로 한번에 처리하는 프레임 수를 다양하게 함 OK
  // - 100MB까지 전송하여 대규모 전송 속도와 정확성 확인. 10MB/sec OK
  // - 직접 close()를 호출해도 안정적으로 처리됨 ?? 정의가 먼저 필요
  // - 에코 패킷 개수를 늘려서 안정성과 처리 성능을 추가로 확인
}