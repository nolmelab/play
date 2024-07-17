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

inline size_t stream_protocol::encode(topic pic, const asio::const_buffer& src,
                                      asio::streambuf& dst)
{
  PLAY_CHECK(!closed_);
  if (closed_)
  {
    LOG()->warn("stream_protocol::send called on closed session. handle: {}", handle_);
    return 0;
  }
  PLAY_CHECK(is_established());

  auto wbuf = dst.prepare(src.size());
  dst.sputn(reinterpret_cast<const char*>(src.data()), src.size());  // auto committed
  return src.size();
}

inline std::tuple<size_t, asio::const_buffer, stream_protocol::topic> stream_protocol::decode(
    const asio::const_buffer& src)
{
  // bypass
  return {src.size(), src, topic{}};
}

std::pair<size_t, asio::const_buffer> stream_protocol::handshake(const asio::const_buffer&)
{
  return {0, {}};
}

}}}  // namespace play::robust::net
