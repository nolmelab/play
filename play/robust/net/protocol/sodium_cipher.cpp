#include <play/robust/net/protocol/sodium_cipher.hpp>

namespace play { namespace robust { namespace net {

std::optional<codec::const_buffer> sodium_cipher::decode(const const_buffer& src_buf)
{
  auto mut_buf = encode_buf_.prepare(src_buf.size());

  const uint8_t* src = reinterpret_cast<const uint8_t*>(src_buf.data());
  uint8_t* dest = reinterpret_cast<uint8_t*>(mut_buf.data());

  int result = crypto_stream_chacha20_xor(
      dest, src, src_buf.size(), handshaker_.get_rx_nonce(), handshaker_.get_rx_key());

  if (result != 0)
  {
    // TODO: error log, throw exception
  }

  encode_buf_.commit(src_buf.size());

  auto recv_buf = encode_buf_.data();
  encode_buf_.consume(src_buf.size());
  return {recv_buf};
}

// encrypt src_buf into dest_buf
size_t sodium_cipher::encode(const const_buffer& src_buf,
                             asio::streambuf& dest_stream_buf)
{
  auto dest_buf = dest_stream_buf.prepare(src_buf.size());

  const uint8_t* src = reinterpret_cast<const uint8_t*>(src_buf.data());
  uint8_t* dest = reinterpret_cast<uint8_t*>(dest_buf.data());

  // chacha20은 padding이 없는 프로토콜인가?
  int result = crypto_stream_chacha20_xor(
      dest, src, src_buf.size(), handshaker_.get_tx_nonce(), handshaker_.get_tx_key());

  if (result != 0)
  {
    // TODO: error log, throw exception
  }

  handshaker_.inc_tx_nonce();

  return 0;
}

}}}  // namespace play::robust::net