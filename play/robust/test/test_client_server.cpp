#include <doctest/doctest.h>
#include <play/robust/net/client.hpp>
#include <play/robust/net/protocol/plain_protocol.hpp>
#include <play/robust/net/server.hpp>

using namespace play::robust::net;

TEST_CASE("client & server")
{
  SUBCASE("compile server")
  {

    server<plain_protocol<uint32_t>> server(R"(
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

  SUBCASE("start server")
  {
    server<plain_protocol<uint32_t>> server(R"(
    {
      "port" : 7000, 
      "concurrency" : 8
    })");

    // auto rc = server.start();

    // runner::sleep(100000);

    // server.stop();
  }

  SUBCASE("compile client")
  {
    //
  }

  SUBCASE("stream_protocol")
  {
    // bypass bytes
    // lookup pattern and make frames
    // 상위 단에서 처리하는 예시를 만들면서 사용성 검토가 됨
  }
}