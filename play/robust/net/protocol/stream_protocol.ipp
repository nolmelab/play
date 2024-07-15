#include <play/robust/base/macros.hpp>
#include <play/robust/net/protocol/stream_protocol.hpp>

namespace play { namespace robust { namespace net {

inline void stream_protocol::accepted()
{
  PLAY_CHECK(!accepted_);
  PLAY_CHECK(!connected_);
  PLAY_CHECK(closed_);

  accepted_ = true;
  closed_ = false;

  get_adapter().established(handle_);
}

inline void stream_protocol::connected()
{
  PLAY_CHECK(!accepted_);
  PLAY_CHECK(!connected_);
  PLAY_CHECK(closed_);

  connected_ = true;
  closed_ = false;

  get_adapter().established(handle_);
}

inline void stream_protocol::closed()
{
  PLAY_RETURN_IF(closed_);
  closed_ = true;
}

inline void stream_protocol::send(const char* data, size_t len)
{
  PLAY_CHECK(!closed_);
  if (closed_)
  {
    LOG()->warn("send called on closed session. handle: {}", handle_);
    return;
  }
  PLAY_CHECK(is_established());

  // bypass
  get_adapter().send(data, len);
}

inline void stream_protocol::receive(const char* data, size_t len)
{
  // bypass
  get_adapter().forward(stream_protocol_topic{}, data, len);
}
}}}  // namespace play::robust::net
