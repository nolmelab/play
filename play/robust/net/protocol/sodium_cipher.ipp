#include <play/robust/base/logger.hpp>
#include <play/robust/net/protocol/sodium_cipher.hpp>

namespace play { namespace robust { namespace net {

// encrypt src_buf into dest_buf
inline size_t sodium_cipher::encode(const asio::const_buffer& src, asio::streambuf& dst)
{
  const uint8_t* rsrc = reinterpret_cast<const uint8_t*>(src.data());
  auto pbuf = dst.prepare(src.size());
  uint8_t* wdst = reinterpret_cast<uint8_t*>(pbuf.data());

  int result = crypto_stream_chacha20_xor(wdst, rsrc, src.size(), handshake_.get_tx_nonce(),
                                          handshake_.get_tx_key());

  if (result != 0)
  {
    auto m = fmt::format("handle: {} failed to encode", handle_);
    LOG()->error(m);
    throw exception(m);
  }

  dst.commit(src.size());
  handshake_.inc_tx_nonce();
  return src.size();
}

inline std::tuple<size_t, asio::const_buffer> sodium_cipher::decode(const asio::const_buffer& src)
{
  uint8_t* buf = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(src.data()));

  // in-place decription
  int result = crypto_stream_chacha20_xor(buf, buf, src.size(), handshake_.get_rx_nonce(),
                                          handshake_.get_rx_key());

  if (result != 0)
  {
    auto m = fmt::format("handle: {} failed to decode", handle_);
    LOG()->error(m);
    throw exception(m);
  }

  handshake_.inc_rx_nonce();

  return {src.size(), asio::const_buffer{buf, src.size()}};
}

}}}  // namespace play::robust::net

// note:
// - crypto_stream_chacha20_xor()은 inplace encryption/decryption이 가능.
// - xor의 뜻은 암복호화에 같이 쓰임
// - streambuf의 prepare()에서 호출하는 reserve가 메모리를 이동하여 확보.
