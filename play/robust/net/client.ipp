#include <play/robust/net/client.hpp>

namespace play { namespace robust { namespace net {

template <typename Protocol>
client<Protocol>::client(runner& runner) : runner_{runner}
{
}

template <typename Protocol>
void client<Protocol>::connect(std::string_view addr, uint16_t port)
{
  if (session_->is_open())
  {
    LOG()->warn("current sesion is open. closing it. handle: {}", session_->get_handle());
    session_->close();
  }

  addr_ = addr;
  port_ = port;

  socket_ = std::make_unique<tcp::socket>();
  auto ip_addr = asio::ip::address::from_string(addr_);
  endpoint_.address(ip_addr);
  endpoint_.port(port);

  boost::asio::async_connect(*socket_, endpoint_,
                             [this](boost::system::error_code ec, tcp::endpoint)
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
  PLAY_CHECK(!!socket_);

  // reuse socket
  boost::asio::async_connect(*socket_, endpoint_,
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
    session_ = std::make_shared<session>(*this, std::move(*socket_), false);
    socket_.reset();  // moved to the session

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
}

template <typename Protocol>
void client<Protocol>::on_closed(session_ptr session, boost::system::error_code ec)
{
  LOG()->info("session closed. handle: {}  remote: {} error: {}", session->get_handle(),
              session->get_endpoint(), ec.message());

  session_.reset();
  socket_.reset();
}

template <typename Protocol>
void client<Protocol>::on_receive(session_ptr session, typename Protocol::topic topic,
                                  const void* data, size_t len)
{
  // TODO: frame_factory<Frame>
}

template <typename Protocol>
void client<Protocol>::start_connect()
{
  acceptor_->async_accept(
      [this](boost::system::error_code ec, tcp::socket socket)
      {
        handle_accept(ec, std::move(socket));
      });
}

}}}  // namespace play::robust::net