#include <doctest/doctest.h>
#include <chrono>
#include <play/robust/base/logger.hpp>
#include <play/robust/net/asio.hpp>
#include <play/robust/net/protocol/length_delimited.hpp>
#include <play/robust/net/protocol/sodium_cipher.hpp>
#include <play/robust/net/protocol/sodium_handshake.hpp>

using namespace play::robust::net;

TEST_CASE("codecs")
{
  SUBCASE("length delimited")
  {
    const size_t payload_size = 10;
    const size_t total_len = payload_size + length_delimited::length_field_size;

    length_delimited codec{1};
    std::array<char, payload_size> payload = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    asio::streambuf send_buf;
    auto dest_buf = send_buf.prepare(total_len);
    auto size = codec.encode(asio::const_buffer{payload.data(), 10}, dest_buf);
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
}
