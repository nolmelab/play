#include <doctest/doctest.h>
#include <chrono>
#include <play/robust/base/logger.hpp>
#include <play/robust/net/asio.hpp>
#include <play/robust/net/protocol/length_delimited.hpp>
//#include <play/robust/net/protocol/sodium_cipher.hpp>
#include <play/robust/net/protocol/sodium_handshake.hpp>

using namespace play::robust::net;

TEST_CASE("codecs")
{
  SUBCASE("length delimited")
  {
    const size_t payload_size = 500;
    const size_t total_len = payload_size + length_delimited::length_field_size;

    length_delimited codec{1};

    std::vector<char> payload;
    for (int i = 0; i < payload_size; ++i)
    {
      payload.push_back('a');
    }

    asio::streambuf send_buf;
    auto dest_buf = send_buf.prepare(total_len);
    auto size = codec.encode(asio::const_buffer{payload.data(), payload_size}, dest_buf);
    CHECK(size == total_len);
    send_buf.commit(total_len);

    auto& recv_buf = send_buf;
    // 현재 읽기 시작점부터 읽기 영역 데이터를 얻는다.
    auto rbuf = recv_buf.data();
    auto frame_buf = codec.decode(rbuf);
    CHECK(frame_buf.size() == payload_size);

    CHECK(std::memcmp(payload.data(), frame_buf.data(), payload_size) == 0);

    // 사용되었다고 가정한다.
    recv_buf.consume(total_len);
    CHECK(recv_buf.data().size() == 0);
  }

  SUBCASE("sodium_hanshake")
  {
    SUBCASE("basics")
    {
      // key와 nonce를 함수 호출로 하듯이 데이터만 교환
      // 길이를 추가하고 해석하는 것은 프로토콜의 역할
      sodium_handshake h1{1, true};
      sodium_handshake h2{2, false};

      auto k1 = h1.prepare();
      auto k2 = h2.prepare();

      auto n1 = h1.on_handshake(k2);
      auto n2 = h2.on_handshake(k1);

      auto [len4, c2] = h2.on_handshake(n1.second);
      auto [len3, c1] = h1.on_handshake(n2.second);

      // [1] 메모리가 깨지는 현상
      // - h1.on_handshake(), h2.on_handshake()에서 h1의 메모리가 깨짐
      // 
    }
  }
}
