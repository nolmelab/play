
#pragma once

#include <optional>
#include <play/robust/net/asio.hpp>
#include <play/robust/net/codec.hpp>

namespace play
{
namespace robust
{
namespace net
{
// sodium 암호화 처리를 위한 정보 교환을 담당한다.
class sodium_handshaker
{
public:
  sodium_handshaker() = default;

  void receive(asio::const_buffer &recv_buf);

private:
  bool key_created_;
  bool key_sent_;
  bool nonce_sent_;
  bool key_received_;
  bool nonce_received_;
};

} // namespace net
} // namespace robust
} // namespace play

#include <play/robust/net/protocol/sodium_handshaker.ipp>