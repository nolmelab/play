#include <play/robust/base/macros.hpp>
#include <play/robust/net/protocol/sodium_handshake.hpp>

namespace play { namespace robust { namespace net {

sodium_handshake::sodium_handshake(size_t handle, bool accepted, send_fn send_fn)
    : handle_{handle},
      accepted_{accepted},
      send_fn_{send_fn},
      key_received_{false},
      nonce_received_{false},
      established_{false}
{
  crypto_kx_keypair(pub_key_, sec_key_);

  uint8_t payload[key_size + 1];
  payload[0] = 'k';
  std::memcpy(payload + 1, pub_key_, key_size);
  send_fn_((const void*)payload, key_size + 1);
}

void sodium_handshake::on_receive(asio::const_buffer& recv_buf)
{
  // reserve buffer for the recv_buf
  recv_stream_buf_.prepare(recv_buf.size());
  recv_stream_buf_.sputn(reinterpret_cast<const char*>(recv_buf.data()), recv_buf.size());
  recv_stream_buf_.commit(recv_buf.size());  // makes it available for reading

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
      auto result =
          crypto_box_seal_open(tx_nonce_, data + 1, size - 1, pub_key_, sec_key_);

      if (result != 0)
      {
        LOG()->warn("handle: {}. failed to open nonce data", handle_);
        throw exception(fmt::format("handle: {}. failed to open nonce data", handle_));
      }
      else
      {
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

      if (accepted_)
      {
        crypto_kx_server_session_keys(rx_key_, tx_key_, pub_key_, sec_key_, data + 1);
      }
      else
      {
        crypto_kx_client_session_keys(rx_key_, tx_key_, pub_key_, sec_key_, data + 1);
      }

      key_received_ = true;

      randombytes_buf(rx_nonce_, nonce_size);

      uint8_t nonce_payload[crypto_box_SEALBYTES + nonce_size + 1];
      nonce_payload[0] = 'n';
      crypto_box_seal(nonce_payload + 1, rx_nonce_, nonce_size, nonce_payload);
      send_fn_((const void*)nonce_payload, sizeof(nonce_payload));
    }

    // 마지막에 consume()으로 읽은 만큼 앞으로 이동
    recv_stream_buf_.consume(payload.size() + length_codec_.length_field_size);
  }
}

}}}  // namespace play::robust::net
