#include <play/robust/net/client.hpp>

namespace play { namespace robust { namespace net {

template <typename Protocol>
client<Protocol>::client(runner& runner) : runner_{runner}, socket_{runner_.get_ioc()}
{
}

template <typename Protocol>
void client<Protocol>::connect(std::string_view addr, uint16_t port)
{
  if (sesion_ && session_->is_open())
  {
    LOG()->warn("current sesion is open. closing it. handle: {}", session_->get_handle());
    session_->close();
    session_.reset();
  }

  addr_ = addr;
  port_ = port;

  auto ip_addr = asio::ip::address::from_string(addr_);
  endpoint_.address(ip_addr);
  endpoint_.port(port);

  session_ = std::make_shared<session>(*this, false);

  boost::asio::async_connect(se->get_socket(), endpoint_,
                             [this, se](boost::system::error_code ec, tcp::endpoint)
                             {
                               handle_connect(ec);
                             });
}

template <typename Protocol>
void client<Protocol>::close()
{
  if (session_->is_open())
  {
    session_->close();
    session_.reset();
  }
}

template <typename Protocol>
void client<Protocol>::reconnect()
{
  PLAY_CHECK(session_);
  PLAY_CHECK(!session_->is_open());

  // reuse socket
  boost::asio::async_connect(socket_, endpoint_,
                             [this](boost::system::error_code ec, tcp::endpoint)
                             {
                               handle_connect(ec);
                             });
}

template <typename Protocol>
void client<Protocol>::start_connect()
{
  PLAY_CHECK(!socket_.is_open())

  boost::asio::async_connect(socket_, endpoint_,
                             [this](boost::system::error_code ec, tcp::endpoint)
                             {
                               handle_connect(ec);
                             });
}

template <typename Protocol>
void client<Protocol>::handle_connect(boost::system::error_code ec)
{
  if (!ec)
  {
    session_->start();

    LOG()->info("session connected. handle: {} remote: {}", session_->get_handle(),
                session_->get_endpoint());
  }
  else
  {
    LOG()->info("connect failed. error: {}", ec.message());
    LOG()->info("reconnecting. addr: {}", addr_);

    reconnect();
  }
}

template <typename Protocol>
void client<Protocol>::on_established(session_ptr session)
{
  LOG()->info("session: {} established", session->get_handle());

  handle_established(session);
}

template <typename Protocol>
void client<Protocol>::on_closed(session_ptr session, boost::system::error_code ec)
{
  LOG()->info("session closed. handle: {}  remote: {} error: {}", session->get_handle(),
              session->get_endpoint(), ec.message());

  handle_closed(session, ec);

  session_.reset();
}

template <typename Protocol>
void client<Protocol>::on_receive(session_ptr session, typename Protocol::topic topic,
                                  const void* data, size_t len)
{
  handle_receive(session, topic, data, len);
}

}}}  // namespace play::robust::net