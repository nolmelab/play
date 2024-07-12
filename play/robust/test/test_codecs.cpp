#include <doctest/doctest.h>
#include <play/robust/net/protocol/length_delimited.hpp>
#include <play/robust/net/asio.hpp>

using namespace play::robust::net;
using namespace boost::asio;

TEST_CASE("codecs")
{
  SUBCASE("length delimited")
  {
    const size_t payload_size = 10;
    const size_t total_len = payload_size + length_delimited::length_field_size;

    length_delimited codec;
    std::array<char, payload_size> payload = { 1, 2, 3, 4, 5 };

    streambuf sbuf;
    auto size = codec.encode(const_buffer{ payload.data(), 10 }, sbuf);
    CHECK(size == total_len);

    // 쓴 바이트들을 읽기 영역으로 이동한다.
    sbuf.commit(total_len);

    // 현재 읽기 시작점부터 읽기 영역 데이터를 얻는다. 
    auto rbuf = sbuf.data();
    auto result = codec.decode(rbuf);
    CHECK(result);
    auto recv_payload = result.value();

    CHECK(std::memcmp(payload.data(), recv_payload.data(), payload_size) == 0);

    // 사용되었다고 가정한다. 
    sbuf.consume(total_len);
    CHECK(sbuf.data().size() == 0);
  }

  SUBCASE("sodium handshaker")
  {

  }

  SUBCASE("sodium cipher")
  {

  }
}