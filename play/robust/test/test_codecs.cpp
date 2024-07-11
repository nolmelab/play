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

    streambuf sbuf;
    length_delimited codec;
    std::array<char, payload_size> payload = { 1, 2, 3, 4, 5 };


    // streambuf prepare는 원하는 크기만큼 확보하여 쓰기 시작인 pptr()부터 돌려준다.
    auto dest_buf = sbuf.prepare(20);
    auto size = codec.encode(const_buffer{ payload.data(), 10 }, dest_buf);
    CHECK(size == total_len);

    const char* p = reinterpret_cast<const char*>(dest_buf.data());
    CHECK(std::memcmp((const void*)(p+4), payload.data(), payload_size) == 0);

    // 쓴 바이트들을 읽기 영역으로 이동한다.
    sbuf.commit(total_len);

    auto rbuf = sbuf.data();
    auto result = codec.decode(rbuf);
    CHECK(result);
    auto recv_payload = result.value();

    CHECK(std::memcmp(payload.data(), recv_payload.data(), payload_size) == 0);
  }

  SUBCASE("sodium handshaker")
  {

  }

  SUBCASE("sodium cipher")
  {

  }
}