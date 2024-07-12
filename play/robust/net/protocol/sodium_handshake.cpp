#include <play/robust/base/logger.hpp>
#include <play/robust/base/macros.hpp>
#include <play/robust/net/protocol/sodium_handshake.hpp>

namespace play { namespace robust { namespace net {

sodium_handshake::sodium_handshake(size_t handle, bool accepted,
                                   send_fn send_fn)
    : handle_{handle},
      accepted_{accepted},
      length_codec_{handle},
      send_fn_{send_fn},
      key_received_{false},
      nonce_received_{false},
      established_{false}
{
}

void sodium_handshake::prepare()
{
  crypto_kx_keypair(pub_key_, sec_key_);
}

void sodium_handshake::sync_key()
{
  uint8_t payload[key_size + 1];
  payload[0] = 'k';
  std::memcpy(payload + 1, pub_key_, key_size);
  send((const void*)payload, key_size + 1);
}

void sodium_handshake::on_receive(asio::const_buffer& recv_buf)
{
  // reserve buffer for the recv_buf
  recv_stream_buf_.prepare(recv_buf.size());
  recv_stream_buf_.sputn(reinterpret_cast<const char*>(recv_buf.data()),
                         recv_buf.size());
  // sputn changes writer pointer, pptr(). commit() 불필요.

  auto buf = recv_stream_buf_.data();  // 읽기 영역 범위를 얻음
  auto result = length_codec_.decode(buf);
  if (result)
  {
    auto payload = result.value();

    const uint8_t* data = reinterpret_cast<const uint8_t*>(payload.data());

    if (key_received_)
    {
      PLAY_CHECK(!nonce_received_);
      PLAY_CHECK(!established_);
      PLAY_CHECK(data[0] == 'n');

      size_t size = payload.size();
      auto result = crypto_box_seal_open(tx_nonce_, data + 1, size - 1,
                                         pub_key_, sec_key_);

      if (result != 0)
      {
        auto m = fmt::format("handle: {}. failed to open nonce data", handle_);
        LOG()->error(m);
        throw exception(m);
      }
      else
      {
        base::logger::dump_hex(spdlog::level::info,
                               fmt::format("handle: {}. tx_nonce", handle_),
                               tx_nonce_, nonce_size);

        nonce_received_ = true;
        established_ = true;
        LOG()->debug("handle: {} established", handle_);
      }
    }
    else
    {
      PLAY_CHECK(!nonce_received_);
      PLAY_CHECK(!established_);
      PLAY_CHECK(data[0] == 'k');

      int result = 0;

      if (accepted_)
      {
        result = crypto_kx_server_session_keys(rx_key_, tx_key_, pub_key_,
                                               sec_key_, data + 1);
      }
      else
      {
        result = crypto_kx_client_session_keys(rx_key_, tx_key_, pub_key_,
                                               sec_key_, data + 1);
      }

      if (result != 0)
      {
        auto m = fmt::format("handle: {} failed to setup sodium keys", handle_);
        LOG()->error(m);
        throw exception(m);
      }

      std::memcpy(peer_pub_key_, data + 1, key_size);

      key_received_ = true;

      randombytes_buf(rx_nonce_, nonce_size);

      base::logger::dump_hex(spdlog::level::info,
                             fmt::format("handle: {}. rx_key", handle_),
                             rx_key_, key_size);
      base::logger::dump_hex(spdlog::level::info,
                             fmt::format("handle: {}. tx_key", handle_),
                             tx_key_, key_size);
      base::logger::dump_hex(spdlog::level::info,
                             fmt::format("handle: {}. rx_nonce", handle_),
                             rx_nonce_, nonce_size);
    }

    // 마지막에 consume()으로 읽은 만큼 앞으로 이동
    recv_stream_buf_.consume(payload.size() + length_codec_.length_field_size);
  }
}

void sodium_handshake::sync_nonce()
{
  uint8_t nonce_payload[crypto_box_SEALBYTES + nonce_size + 1];
  nonce_payload[0] = 'n';
  crypto_box_seal(nonce_payload + 1, rx_nonce_, nonce_size, peer_pub_key_);
  send((const void*)nonce_payload, sizeof(nonce_payload));
}

void sodium_handshake::send(const void* data, size_t len)
{
  length_codec_.encode(asio::const_buffer{data, len}, send_stream_buf_);

  auto payload = send_stream_buf_.data();
  send_fn_(payload.data(), payload.size());

  send_stream_buf_.consume(payload.size());
}

}}}  // namespace play::robust::net
