#include <play/base/logger.hpp>
#include <play/base/macros.hpp>
#include <play/net/protocol/sodium_handshake.hpp>
#include <string_view>

namespace play {

inline sodium_handshake::sodium_handshake(size_t handle, bool accepted)
    : handle_{handle},
      accepted_{accepted},
      key_received_{false},
      nonce_received_{false},
      established_{false}
{
}

inline asio::const_buffer sodium_handshake::prepare()
{
  crypto_kx_keypair(pub_key_, sec_key_);
  return sync_key();
}

inline std::pair<size_t, asio::const_buffer> sodium_handshake::on_handshake(asio::const_buffer& src)
{
  PLAY_CHECK(src.size() > 0);
  PLAY_CHECK(src.size() == key_size + 1 || src.size() == nonce_exchange_size);

  const uint8_t* data = reinterpret_cast<const uint8_t*>(src.data());

  if (key_received_)
  {
    PLAY_CHECK(!nonce_received_);
    PLAY_CHECK(!established_);
    PLAY_CHECK(data[0] == 'n');

    auto result = crypto_box_seal_open(tx_nonce_, data + 1, src.size() - 1, pub_key_, sec_key_);

    if (result != 0)
    {
      auto m = fmt::format("handle: {}. failed to open nonce data", handle_);
      LOG()->error(m);
      throw exception(m);
    }
    else
    {
      nonce_received_ = true;
      established_ = true;
      LOG()->debug("handle: {} established", handle_);
    }

    return {src.size(), {}};  // 완료된 상태로 더 이상 전송할 내용 없음
  }
  // else

  PLAY_CHECK(!nonce_received_);
  PLAY_CHECK(!established_);
  PLAY_CHECK(data[0] == 'k');

  int result = 0;

  std::memcpy(peer_pub_key_, data + 1, key_size);

  if (accepted_)
  {
    result = crypto_kx_server_session_keys(rx_key_, tx_key_, pub_key_, sec_key_, peer_pub_key_);
  }
  else
  {
    result = crypto_kx_client_session_keys(rx_key_, tx_key_, pub_key_, sec_key_, peer_pub_key_);
  }

  if (result != 0)
  {
    auto m = fmt::format("handle: {} failed to setup sodium keys", handle_);
    LOG()->error(m);
    throw exception(m);
  }

  key_received_ = true;

  randombytes_buf(rx_nonce_, nonce_size);

  // 세션에서 받은 데이터를 재해석하므로 받은만큼 consume하도록 함
  return {src.size(), sync_nonce()};
}

inline asio::const_buffer sodium_handshake::sync_key()
{
  key_exchange_buf_[0] = 'k';
  std::memcpy(key_exchange_buf_ + 1, pub_key_, key_size);
  return {(const void*)key_exchange_buf_, sizeof(key_exchange_buf_)};
}

inline asio::const_buffer sodium_handshake::sync_nonce()
{
  // nonc_exchange_size에 seal 바이트를 포함. 메모리 문제를 포함한 다양한 문제의 원인
  nonce_exchange_buf_[0] = 'n';
  crypto_box_seal(nonce_exchange_buf_ + 1, rx_nonce_, nonce_size, peer_pub_key_);

  return {(const void*)nonce_exchange_buf_, sizeof(nonce_exchange_buf_)};
}

inline void sodium_handshake::dump_state(std::string_view step)
{
  LOG()->debug("handshake step: {}", step);

  logger::dump_hex(spdlog::level::info, fmt::format("handle: {}. pub_key", handle_), pub_key_,
                   key_size);

  logger::dump_hex(spdlog::level::info, fmt::format("handle: {}. peer_pub_key", handle_),
                   peer_pub_key_, key_size);

  logger::dump_hex(spdlog::level::info, fmt::format("handle: {}. rx_key", handle_), rx_key_,
                   key_size);

  logger::dump_hex(spdlog::level::info, fmt::format("handle: {}. tx_key", handle_), tx_key_,
                   key_size);

  logger::dump_hex(spdlog::level::info, fmt::format("handle: {}. rx_nonce", handle_), rx_nonce_,
                   nonce_size);

  logger::dump_hex(spdlog::level::info, fmt::format("handle: {}. tx_nonce", handle_), tx_nonce_,
                   nonce_size);
}

}  // namespace play
