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

    asio::streambuf sbuf;
    auto size = codec.encode(asio::const_buffer{payload.data(), 10}, sbuf);
    CHECK(size == total_len);

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
}

namespace {

class dummy_handshake
{
 public:
  dummy_handshake() = default;

  void create(size_t handle, bool accepted, sodium_handshake::send_fn fn)
  {
    handshake_ = std::make_unique<sodium_handshake>(handle, accepted, fn);
  }

  void receive_from_peer(const void* data, size_t len)
  {
    asio::const_buffer recv_buf{data, len};
    handshake_->on_receive(recv_buf);
  }

  sodium_handshake& get_handshake() const { return *handshake_.get(); }

 private:
  std::unique_ptr<sodium_handshake> handshake_;
};

// 암호화 통신을 하는 클라/서버를 대신.
/**
 * - dummy_handshake를 사용하여 암호 협상 진행 
 * - streambuf를 통해 length_delimited 코덱을 통한 페이로드 받음 
 * - send() 함수를 제공하여 서로 통신
 */
class dummy_peer
{
 public:
  dummy_peer() = default;

  void start(size_t handle, sodium_handshake& handshake)
  {
    cipher_codec_ = std::make_unique<sodium_cipher>(handle, handshake);
    length_codec_ = std::make_unique<length_delimited>(handle);

    messages_.push_back("empty");
  }

  void receive_from_peer(const void* data, size_t len)
  {
    const char* payload = reinterpret_cast<const char*>(data);
    auto mut_buf = recv_buf_.prepare(len);
    recv_buf_.sputn(payload, len);
    // commit() 불필요

    auto data_buf = recv_buf_.data();
    // 프레임이 있으면 계속 읽음
    while (auto result = length_codec_->decode(data_buf))
    {
      auto cipher_buf = result.value();
      auto plain_result = cipher_codec_->decode(cipher_buf);

      if (plain_result)
      {
        auto final_data = plain_result.value();
        const char* s = reinterpret_cast<const char*>(final_data.data());
        messages_.front() = (std::string(s, final_data.size() - 1));
      }

      // 다음 사용 가능한 데이터로 이동
      recv_buf_.consume(cipher_buf.size() + length_delimited::length_field_size);
      data_buf = recv_buf_.data();
    }
  }

  void send_to_peer(const void* data, size_t len, codec::send_fn fn)
  {
    auto payload_buf = asio::const_buffer{data, len};
    auto enc_len = cipher_codec_->encode(payload_buf, enc_buf_);
    if (enc_len > 0)
    {
      auto enc_buf = enc_buf_.data();
      auto result = length_codec_->encode(enc_buf, send_buf_);
      if (result)
      {
        auto send_data = send_buf_.data();
        fn(send_data.data(), send_data.size());
        send_buf_.consume(send_data.size());
      }

      // 사용한 만큼 읽기 포인터를 앞으로 이동해야 함
      enc_buf_.consume(enc_buf.size());
    }
  }

  auto get_messages() { return messages_; }

 private:
  std::unique_ptr<sodium_cipher> cipher_codec_;
  std::unique_ptr<length_delimited> length_codec_;
  asio::streambuf recv_buf_;
  asio::streambuf enc_buf_;
  asio::streambuf send_buf_;
  std::vector<std::string> messages_;
};

}  // namespace

TEST_CASE("sodium")
{

  SUBCASE("sodium handshake")
  {
    dummy_handshake c1;
    dummy_handshake c2;

    c1.create(1, true,
              [&c2](const void* data, size_t len)
              {
                c2.receive_from_peer(data, len);
              });
    c2.create(2, false,
              [&c1](const void* data, size_t len)
              {
                c1.receive_from_peer(data, len);
              });

    c1.get_handshake().prepare();
    c2.get_handshake().prepare();

    c1.get_handshake().sync_key();
    c2.get_handshake().sync_key();

    c1.get_handshake().sync_nonce();
    c2.get_handshake().sync_nonce();

    CHECK(c1.get_handshake().is_established());
    CHECK(c2.get_handshake().is_established());
  }

  SUBCASE("sodium cipher")
  {
    LOG()->info("sodium cipher");

    dummy_handshake c1;
    dummy_handshake c2;

    c1.create(1, true,
              [&c2](const void* data, size_t len)
              {
                c2.receive_from_peer(data, len);
              });
    c2.create(2, false,
              [&c1](const void* data, size_t len)
              {
                c1.receive_from_peer(data, len);
              });

    c1.get_handshake().prepare();
    c2.get_handshake().prepare();

    c1.get_handshake().sync_key();
    c2.get_handshake().sync_key();

    c1.get_handshake().sync_nonce();
    c2.get_handshake().sync_nonce();

    CHECK(c1.get_handshake().is_established());
    CHECK(c2.get_handshake().is_established());

    dummy_peer p1;
    dummy_peer p2;

    p1.start(1, c1.get_handshake());
    p2.start(2, c2.get_handshake());  // c2를 c1으로 하여 2시간 헤맸다.

    const int test_count = 10;

    std::chrono::time_point start = std::chrono::steady_clock::now();

    for (int i = 0; i < test_count; ++i)
    {
      auto m = fmt::format("hello {}", i);
      p2.send_to_peer(reinterpret_cast<const void*>(m.c_str()), m.length() + 1,
                      [&p1](const void* data, size_t len)
                      {
                        p1.receive_from_peer(data, len);
                      });

      const auto& messages = p1.get_messages();
      bool result = messages.front() == m;
      CHECK(result);
    }

    std::chrono::time_point end = std::chrono::steady_clock::now();

    auto diff = end - start;
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();

    LOG()->info("send_to_peer. elapsed: {}, count: {}", elapsed, test_count);
    // 초당 3백만 건 정도를 처리한다. dummy 코드이므로 최적의 구현에서는 더 빠를 듯.
  }
}