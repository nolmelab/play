#include <play/net/client.hpp>

namespace play {

template <typename Protocol>
client<Protocol>::client(runner& runner)
    : runner_{runner}
{
}

template <typename Protocol>
inline bool client<Protocol>::connect(std::string_view addr, uint16_t port)
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

template <typename Protocol>
inline void client<Protocol>::close()
{
  if (session_->is_open())
  {
    session_->close();
    session_.reset();

    // XXX: 앱에서 연결 종료를 더 기다려야 하는가? on_closed()를 대기?
  }
}

template <typename Protocol>
inline void client<Protocol>::reconnect()
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

template <typename Protocol>
inline void client<Protocol>::handle_connect(error_code ec)
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

template <typename Protocol>
void client<Protocol>::bind_pulse(pulse_listener* listener)
{
  PLAY_CHECK(listener != nullptr);
  listener_ = listener;
}

template <typename Protocol>
inline void client<Protocol>::on_established(session_ptr session)
{
  LOG()->info("client session: {} established. remote: {}", session->get_handle(),
              session->get_remote_addr());

  handle_established(session);
  if (listener_ != nullptr)
  {
    listener_->on_established(session);
  }
}

template <typename Protocol>
inline void client<Protocol>::on_closed(session_ptr session, error_code ec)
{
  LOG()->info("client session closed. handle: {}  remote: {} error: {}", session->get_handle(),
              session->get_remote_addr(), ec.message());

  handle_closed(session, ec);

  if (listener_ != nullptr)
  {
    listener_->on_closed(session, ec);
  }

  session_.reset();
}

template <typename Protocol>
inline void client<Protocol>::on_receive(session_ptr session, topic topic, const void* data,
                                         size_t len)
{
  if (listener_ != nullptr)
  {
    listener_->on_receive(session, topic, data, len);
  }
  handle_receive(session, topic, data, len);
}

}  // namespace play