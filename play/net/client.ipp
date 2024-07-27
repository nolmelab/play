#include <play/net/client.hpp>

namespace play {

template <typename Protocol, typename Frame>
client<Protocol, Frame>::client(frame_handler& handler)
    : runner_{handler.get_runner()},
      frame_handler_{handler}
{
}

template <typename Protocol, typename Frame>
inline bool client<Protocol, Frame>::connect(std::string_view addr, uint16_t port)
{
  if (session_ && session_->is_open())
  {
    LOG()->warn("current sesion is open. closing it. handle: {}", session_->get_handle());
    return false;
  }

  addr_ = addr;
  port_ = port;

  auto ip_addr = asio::ip::address::from_string(addr_);
  endpoint_.address(ip_addr);
  endpoint_.port(port);

  session_ = std::make_shared<session>(*this, runner_.get_ioc(), false);

  session_->get_socket().async_connect(endpoint_,
                                       [this](error_code ec)
                                       {
                                         handle_connect(ec);
                                       });

  return true;
}

template <typename Protocol, typename Frame>
template <typename FrameHandler>
FrameHandler& client<Protocol, Frame>::get_handler() const
{
  return static_cast<FrameHandler&>(frame_handler_);
}

template <typename Protocol, typename Frame>
inline void client<Protocol, Frame>::close()
{
  if (session_->is_open())
  {
    session_->close();
    session_.reset();

    // XXX: 앱에서 연결 종료를 더 기다려야 하는가? on_closed()를 대기?
  }
}

template <typename Protocol, typename Frame>
inline void client<Protocol, Frame>::reconnect()
{
  PLAY_CHECK(session_);
  PLAY_CHECK(!session_->is_open());

  // reuse socket
  session_->get_socket().async_connect(endpoint_,
                                       [this](error_code ec)
                                       {
                                         handle_connect(ec);
                                       });
}

template <typename Protocol, typename Frame>
inline void client<Protocol, Frame>::handle_connect(error_code ec)
{
  if (!ec)
  {
    session_->start();

    LOG()->info("session connected. handle: {} remote: {}", session_->get_handle(),
                session_->get_remote_addr());
  }
  else
  {
    LOG()->info("connect failed. error: {}", ec.message());
    LOG()->info("reconnecting. addr: {}", addr_);

    reconnect();
  }
}

template <typename Protocol, typename Frame>
inline void client<Protocol, Frame>::on_established(session_ptr session)
{
  LOG()->info("client session: {} established. remote: {}", session->get_handle(),
              session->get_remote_addr());

  handle_established(session);
  frame_handler_.established(session);
}

template <typename Protocol, typename Frame>
inline void client<Protocol, Frame>::on_closed(session_ptr session, error_code ec)
{
  LOG()->info("client session closed. handle: {}  remote: {} error: {}", session->get_handle(),
              session->get_remote_addr(), ec.message());

  handle_closed(session, ec);
  frame_handler_.closed(session, ec);

  session_.reset();
}

template <typename Protocol, typename Frame>
inline void client<Protocol, Frame>::on_receive(session_ptr session, topic topic, const void* data,
                                                size_t len)
{
  frame_handler_.recv(session, topic, data, len);
  handle_receive(session, topic, data, len);
}

}  // namespace play