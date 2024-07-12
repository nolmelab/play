
#pragma once

#include <sodium.h>
#include <optional>
#include <play/robust/net/asio.hpp>
#include <play/robust/net/protocol/length_delimited.hpp>

namespace play { namespace robust { namespace net {

// sodium 암호화 처리를 위한 정보 교환을 담당한다.
class sodium_handshake
{
 public:
  using send_fn = std::function<void(const void* data, size_t len)>;

  struct exception : public std::exception
  {
    exception(const std::string& what) : what_{what} {}

    const char* what() const noexcept override { return what_.c_str(); }

    std::string what_;
  };

 public:
  static const size_t key_size = crypto_stream_chacha20_KEYBYTES;
  static const size_t nonce_size = crypto_stream_chacha20_NONCEBYTES;

 public:
  sodium_handshake(size_t handle, bool accepted, send_fn send_fn);

  // recv from peer
  /**
   * @throws potocol_exception이 협상 실패시 발생
   */
  void on_receive(asio::const_buffer& recv_buf);

  bool is_established() const { return established_; }

  const uint8_t* get_pub_key() const { return pub_key_; }

  const uint8_t* get_sec_key() const { return sec_key_; }

  const uint8_t* get_rx_key() const { return rx_key_; }

  const uint8_t* get_tx_key() const { return tx_key_; }

  const uint8_t* get_rx_nonce() const { return rx_nonce_; }

  const uint8_t* get_tx_nonce() const { return tx_nonce_; }

  void inc_tx_nonce() { sodium_increment(tx_nonce_, nonce_size); }

  void inc_rx_nonce() { sodium_increment(rx_nonce_, nonce_size); }

 private:
  size_t handle_;
  bool accepted_;
  send_fn send_fn_;

  bool key_received_;
  bool nonce_received_;
  bool established_;

  asio::streambuf recv_stream_buf_;
  length_delimited length_codec_;

  uint8_t pub_key_[key_size];
  uint8_t sec_key_[key_size];

  uint8_t rx_key_[key_size];
  uint8_t tx_key_[key_size];
  uint8_t rx_nonce_[nonce_size];
  uint8_t tx_nonce_[nonce_size];
};

}}}  // namespace play::robust::net
