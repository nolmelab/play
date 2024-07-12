#include <play/robust/base/logger.hpp>
#include <play/robust/net/protocol/sodium_cipher.hpp>

namespace play { namespace robust { namespace net {

std::optional<codec::const_buffer> sodium_cipher::decode(const const_buffer& src_buf)
{
  auto mut_buf = encode_buf_.prepare(src_buf.size());

  const uint8_t* src = reinterpret_cast<const uint8_t*>(src_buf.data());
  uint8_t* dest = reinterpret_cast<uint8_t*>(mut_buf.data());

  int result = crypto_stream_chacha20_xor(
      dest, src, src_buf.size(), handshake_.get_rx_nonce(), handshake_.get_rx_key());

  if (result != 0)
  {
    auto m = fmt::format("handle: {} failed to decode", handle_);
    LOG()->error(m);
    throw exception(m);
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

  int result = crypto_stream_chacha20_xor(
      dest, src, src_buf.size(), handshake_.get_tx_nonce(), handshake_.get_tx_key());

  if (result != 0)
  {
    auto m = fmt::format("handle: {} failed to encode", handle_);
    LOG()->error(m);
    throw exception(m);
  }

  // make the payload available for reading
  dest_stream_buf.commit(src_buf.size());

  handshake_.inc_tx_nonce();

  return src_buf.size();
}

}}}  // namespace play::robust::net

// note:
// - crypto_stream_chacha20_xor()은 inplace encryption/decryption이 가능.
// - xor의 뜻은 암복호화에 같이 쓰임
// - streambuf의 prepare()에서 호출하는 reserve가 메모리를 이동하여 확보.
