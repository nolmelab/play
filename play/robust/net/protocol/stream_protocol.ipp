#include <play/robust/base/macros.hpp>
#include <play/robust/net/protocol/stream_protocol.hpp>

namespace play { namespace robust { namespace net {

inline asio::const_buffer stream_protocol::accepted()
{
  PLAY_CHECK(!accepted_);
  PLAY_CHECK(!connected_);
  PLAY_CHECK(closed_);

  accepted_ = true;
  closed_ = false;

  return {};
}

inline asio::const_buffer stream_protocol::connected()
{
  PLAY_CHECK(!accepted_);
  PLAY_CHECK(!connected_);
  PLAY_CHECK(closed_);

  connected_ = true;
  closed_ = false;

  return {};
}

inline void stream_protocol::closed()
{
  PLAY_RETURN_IF(closed_);
  closed_ = true;
}

inline size_t stream_protocol::encode(topic pic, const char* data, size_t len,
                                      asio::streambuf& dest)
{
  PLAY_CHECK(!closed_);
  if (closed_)
  {
    LOG()->warn("stream_protocol::send called on closed session. handle: {}", handle_);
    return 0;
  }

  PLAY_CHECK(is_established());

  auto buf = dest.prepare(len);
  dest.sputn(data, len);  // auto committed

  return len;
}

inline std::tuple<size_t, asio::const_buffer, stream_protocol::topic> stream_protocol::decode(
    const char* data, size_t len)
{
  // bypass
  return {len, asio::const_buffer{data, len}, {}};
}

std::pair<size_t, asio::const_buffer> stream_protocol::handshake(const char*, size_t)
{
  return {0, {}};
}

}}}  // namespace play::robust::net
