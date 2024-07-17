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
}

struct host
{
  static const size_t key_size = crypto_stream_chacha20_KEYBYTES;
  static const size_t nonce_size = crypto_stream_chacha20_NONCEBYTES;

  uint8_t pub_key_[key_size];
  uint8_t sec_key_[key_size];

  uint8_t rx_key_[key_size];
  uint8_t tx_key_[key_size];
  uint8_t rx_nonce_[nonce_size];
  uint8_t tx_nonce_[nonce_size];

  uint8_t key_exchange_buf_[key_size];
  uint8_t nonce_exchange_buf_[nonce_size + crypto_box_SEALBYTES];
};

TEST_CASE("sodium_hanshake")
{
  // 사용하려는 인터페이스나 알고리즘의 동작을 사전에 확인하면 좋다.
  // 문서보다 더 잘 설명할 수 있고, 잘못 쓰는 걸 막아주며, 버그를 찾을 때도 도움이 된다.
  SUBCASE("manual flow")
  {
    host s;
    host c;

    crypto_kx_keypair(s.pub_key_, s.sec_key_);
    crypto_kx_keypair(c.pub_key_, c.sec_key_);

    {
      auto result =
          crypto_kx_server_session_keys(s.rx_key_, s.tx_key_, s.pub_key_, s.sec_key_, c.pub_key_);
      CHECK(result == 0);
    }

    {
      auto result =
          crypto_kx_client_session_keys(c.rx_key_, c.tx_key_, c.pub_key_, c.sec_key_, s.pub_key_);
      CHECK(result == 0);
    }
    // server, client 페어를 잘 맞추면 동작
    CHECK(std::memcmp(s.tx_key_, c.rx_key_, host::key_size) == 0);
    CHECK(std::memcmp(s.rx_key_, c.tx_key_, host::key_size) == 0);

    uint8_t test_buf[5 + crypto_box_SEALBYTES];
    uint8_t recv_buf[5];

    // public 키로 seal 한다. sodium 문서에서 recipient로 같은 이유
    crypto_box_seal(test_buf, (const uint8_t*)"hello", 5, c.pub_key_);
    {
      // public, secrete 키로 open 한다.
      auto result = crypto_box_seal_open(recv_buf, test_buf, 5 + crypto_box_SEALBYTES, c.pub_key_,
                                         c.sec_key_);
      CHECK(result == 0);
    }
    // - ERROR: 위 데이터도 open에서 실패한다.
    //   - 뭔가 잘못 이해하고 있는 부분이 있나 보다

    randombytes_buf(s.rx_nonce_, host::nonce_size);
    randombytes_buf(c.rx_nonce_, host::nonce_size);

    crypto_box_seal(c.nonce_exchange_buf_, c.rx_nonce_, host::nonce_size, s.pub_key_);
    {
      auto result =
          crypto_box_seal_open(s.tx_nonce_, c.nonce_exchange_buf_,
                               host::nonce_size + crypto_box_SEALBYTES, s.pub_key_, s.sec_key_);
      CHECK(result == 0);
    }

    crypto_box_seal(s.nonce_exchange_buf_, s.rx_nonce_, host::nonce_size, c.pub_key_);
    {
      auto result =
          crypto_box_seal_open(c.tx_nonce_, s.nonce_exchange_buf_,
                               host::nonce_size + crypto_box_SEALBYTES, c.pub_key_, c.sec_key_);
      CHECK(result == 0);
    }
  }

  SUBCASE("basics")
  {
    // key와 nonce를 함수 호출로 하듯이 데이터만 교환
    // 길이를 추가하고 해석하는 것은 프로토콜의 역할
    sodium_handshake h1(1, true);
    sodium_handshake h2(2, false);

    auto k1 = h1.prepare();
    auto k2 = h2.prepare();

    auto n1 = h1.on_handshake(k2);
    auto n2 = h2.on_handshake(k1);

    auto [len4, c2] = h2.on_handshake(n1.second);
    auto [len3, c1] = h1.on_handshake(n2.second);

    CHECK(h1.is_established());
    CHECK(h2.is_established());
  }

  SUBCASE("sodium_cipher")
  {
    sodium_handshake h1(1, true);
    sodium_handshake h2(2, false);

    auto k1 = h1.prepare();
    auto k2 = h2.prepare();

    auto n1 = h1.on_handshake(k2);
    auto n2 = h2.on_handshake(k1);

    auto [len4, c2] = h2.on_handshake(n1.second);
    auto [len3, c1] = h1.on_handshake(n2.second);

    CHECK(h1.is_established());
    CHECK(h2.is_established());

    sodium_cipher cipher1(1, h1);
    sodium_cipher cipher2(2, h2);

    asio::streambuf b1;
    asio::streambuf b2;

    std::vector<char> vs;
    for (int i = 0; i < 1024; ++i)
    {
      vs.push_back('a');
    }

    asio::const_buffer p1{vs.data(), vs.size()};

    auto len = cipher1.encode(p1, b1);
    CHECK(len == b1.size());

    auto rbuf = b1.data();
    auto [l_2, frame] = cipher2.decode(rbuf);
    CHECK(l_2 == frame.size());
    b1.consume(l_2);

    CHECK(std::memcmp(frame.data(), p1.data(), p1.size()) == 0);
  }
}

// sodium_handshake:
// [1] 메모리가 깨지는 현상
// - h1.on_handshake(), h2.on_handshake()에서 h1의 메모리가 깨짐
// - -fstack-protector-all로 확인 안 됨
//   - debugging이 안 돼서 -g 옵션 추가
// - valgrind를 WSL에 설치하고 실행했으나 디버그 정보 오류가 나옴
// [2] unique_ptr로 바꾸고 nonce open에 실패
// - 키 값들은 다 맞는데 그런다.
// -
// 여러가지 문제의 원인이 crypto_box_SEALBYTES를 누락해서 생긴 문제.
// C++에서 이와 같은 문제가 쉽게 드러나지 않는다.
// Ubuntu + clang에서 더 정확하게 동작한다. 바로 죽어준다.

// sodium_cipher:
// - 바로 된 듯 하나 secure_protocol로 확인해야 한다.
