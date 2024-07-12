#include <doctest/doctest.h>
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

    length_delimited codec;
    std::array<char, payload_size> payload = {1, 2, 3, 4, 5};

    asio::streambuf sbuf;
    auto size = codec.encode(asio::const_buffer{payload.data(), 10}, sbuf);
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
}

namespace {

class dummy_handshake
{
 public:
  dummy_handshake() = default;

  void start(size_t handle, bool accepted, sodium_handshake::send_fn fn)
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
    length_codec_ = std::make_unique<length_delimited>();
  }

  void receive_from_peer(const void* data, size_t len)
  {
    const char* payload = reinterpret_cast<const char*>(data);
    auto mut_buf = recv_buf_.prepare(len);
    recv_buf_.sputn(payload, len);
    recv_buf_.commit(len);  // make it available

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
        messages_.push_back(std::string(s, final_data.size()));
      }
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
      }

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

    c1.start(1, true, [&c2](const void* data, size_t len) { c2.receive_from_peer(data, len); });
    c2.start(2, false, [&c1](const void* data, size_t len) { c1.receive_from_peer(data, len); });

    CHECK(c1.get_handshake().is_established());
    CHECK(c2.get_handshake().is_established());
  }

  SUBCASE("sodium cipher")
  {
    dummy_handshake c1;
    dummy_handshake c2;

    c1.start(1, true, [&c2](const void* data, size_t len) { c2.receive_from_peer(data, len); });
    c2.start(2, false, [&c1](const void* data, size_t len) { c1.receive_from_peer(data, len); });

    CHECK(c1.get_handshake().is_established());
    CHECK(c2.get_handshake().is_established());

    dummy_peer p1;
    dummy_peer p2;

    p1.start(1, c1.get_handshake());
    p2.start(2, c1.get_handshake());

    auto m = fmt::format("hello {}", 1);
    p2.send_to_peer(reinterpret_cast<const void*>(m.c_str()), m.length() + 1,
                    [&p1](const void* data, size_t len) { p1.receive_from_peer(data, len); });

    CHECK(p1.get_messages()[0] == m);
  }
}