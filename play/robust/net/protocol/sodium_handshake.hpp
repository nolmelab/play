
#pragma once

#include <sodium.h>
#include <optional>
#include <play/robust/net/asio.hpp>
#include <play/robust/net/protocol/length_delimited.hpp>

namespace play { namespace robust { namespace net {

// sodium 암호화 처리를 위한 정보 교환을 담당한다.
/**
 * 공개키 알고리즘의 키를 전달 
 * 상대편 키를 포함하여 대칭키 쌍을 생성 (같은 송수신 키 짝으로 생성됨)
 * 수신 nonce를 전달하여 전송 nonce로 사용
 * 교환된 값으로 대칭키 암호화를 가능한 상태가 되면 established가 됨
 */
class sodium_handshake
{
public:
  struct exception : public std::exception
  {
    exception(const std::string& what)
        : what_{what}
    {
    }

    const char* what() const noexcept override
    {
      return what_.c_str();
    }

    std::string what_;
  };

public:
  static const size_t key_size = crypto_stream_chacha20_KEYBYTES;
  static const size_t nonce_size = crypto_stream_chacha20_NONCEBYTES;

public:
  sodium_handshake(size_t handle, bool accepted);

  // 공개키를 생성
  asio::const_buffer prepare();

  // recv from peer
  /**
   * length_delimited 코덱으로 데이터를 만들어 돌려줌
   * 
   * @throws potocol_exception이 협상 실패시 발생
   * @return <consumed_len, handshake data buffer>를 돌려줌
   */
  std::pair<size_t, asio::const_buffer> on_handshake(asio::const_buffer& recv_buf);

  bool is_key_received() const
  {
    return key_received_;
  }

  bool is_established() const
  {
    return established_;
  }

  const uint8_t* get_pub_key() const
  {
    return pub_key_;
  }

  const uint8_t* get_sec_key() const
  {
    return sec_key_;
  }

  const uint8_t* get_rx_key() const
  {
    return rx_key_;
  }

  const uint8_t* get_tx_key() const
  {
    return tx_key_;
  }

  const uint8_t* get_rx_nonce() const
  {
    return rx_nonce_;
  }

  const uint8_t* get_tx_nonce() const
  {
    return tx_nonce_;
  }

  void inc_tx_nonce()
  {
    sodium_increment(tx_nonce_, nonce_size);
  }

  void inc_rx_nonce()
  {
    sodium_increment(rx_nonce_, nonce_size);
  }

private:
  // 피어에 공개키 전송
  asio::const_buffer sync_key();

  // 피어에 rx_nonce를 전송하여 tx_nonce로 쓸 수 있게 함
  asio::const_buffer sync_nonce();

  void dump_state();

private:
  size_t handle_;
  bool accepted_;

  bool key_received_;
  bool nonce_received_;
  bool established_;

  uint8_t pub_key_[key_size];
  uint8_t sec_key_[key_size];
  uint8_t peer_pub_key_[key_size];

  uint8_t rx_key_[key_size];
  uint8_t tx_key_[key_size];
  uint8_t rx_nonce_[nonce_size];
  uint8_t tx_nonce_[nonce_size];

  uint8_t key_exchange_buf_[key_size + 1];
  uint8_t nonce_exchange_buf_[nonce_size + 1];
};

}}}  // namespace play::robust::net
