#include <play/robust/net/server.hpp>

namespace play { namespace robust { namespace net {

template <typename Protocol>
server<Protocol>::server(runner& runner, std::string_view json) : runner_{runner}, json_{json}
{
}

template <typename Protocol>
bool server<Protocol>::start()
{
  LOG()->info("server starting...");

  try
  {
    jconf_ = nlohmann::json::parse(json_);

    auto port = jconf_["listen"].get<uint16_t>();
    auto endpoint = tcp::endpoint{{}, port};

    acceptor_ = std::make_unique<acceptor>(runner_.get_ioc());
    acceptor_->open(asio::ip::tcp::v4());
    acceptor_->bind(endpoint);
    acceptor_->listen();

    start_accept();

    LOG()->info("server listening on port: {}", port);
  }
  catch (std::exception& ex)
  {
    LOG()->error("exception: {} while starting server", ex.what());
    return false;
  }

  return on_start();
}

template <typename Protocol>
typename server<Protocol>::session_ptr server<Protocol>::get_session(size_t handle)
{
  std::shared_lock<shared_mutex> guard(mutex_);
  auto iter = sessions_.find(handle);
  if (iter == sessions_.end())
  {
    return {};
  }
  return iter->second;
}

template <typename Protocol>
void server<Protocol>::stop()
{
  on_stop();

  if (!!acceptor_)
  {
    acceptor_.reset();
  }

  LOG()->info("server stopped");
}

template <typename Protocol>
bool server<Protocol>::on_start()
{
  return true;
}

template <typename Protocol>
void server<Protocol>::on_stop()
{
}

template <typename Protocol>
void server<Protocol>::on_established(session_ptr session)
{
  LOG()->info("session: {} established", session->get_handle());

  handle_established(session);
}

template <typename Protocol>
void server<Protocol>::on_closed(session_ptr session, boost::system::error_code ec)
{
  // remove
  {
    std::unique_lock<shared_mutex> guard(mutex_);
    sessions_.erase(session->get_handle());
  }

  LOG()->info("session closed. handle: {}  remote: {} error: {}", session->get_handle(),
              session->get_endpoint(), ec.message());

  handle_closed(session, ec);
}

template <typename Protocol>
void server<Protocol>::on_receive(session_ptr session, topic topic, const void* data, size_t len)
{
  handle_receive(session, topic, data, len);
}

template <typename Protocol>
void server<Protocol>::start_accept()
{
  auto session = std::make_shared<session>(*this, true);

  acceptor_->async_accept(
      [this](boost::system::error_code ec, tcp::socket socket)
      {
        handle_accept(ec, std::move(socket));
      });
}

template <typename Protocol>
void server<Protocol>::handle_accept(session_ptr se, boost::system::error_code ec)
{
  if (!ec)
  {
    // add
    {
      std::unique_lock<shared_mutex> guard(mutex_);
      sessions_.insert(std::pair(ss->get_handle(), se));
    }

    se->start();

    LOG()->info("session accepted. remote: {}", ss->get_endpoint());
  }
  else
  {
    LOG()->info("accept error: {}", ec.message());
  }

  start_accept();
}

}}}  // namespace play::robust::net