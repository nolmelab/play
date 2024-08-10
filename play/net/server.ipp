#include <play/net/server.hpp>

namespace play {

template <typename Protocol>
server<Protocol>::server(runner& runner)
    : runner_{runner}
{
}

template <typename Protocol>
bool server<Protocol>::start(uint16_t port)
{
  try
  {
    auto endpoint = tcp::endpoint{{}, port};

    acceptor_ = std::make_unique<acceptor>(runner_.get_ioc());
    acceptor_->open(asio::ip::tcp::v4());
    acceptor_->set_option(tcp::acceptor::reuse_address(true));  //
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
void server<Protocol>::bind_pulse(pulse_listener* listener)
{
  PLAY_CHECK(listener != nullptr);
  listener_ = listener;
}

template <typename Protocol>
void server<Protocol>::on_established(session_ptr session)
{
  LOG()->info("server session: {} established. remote: {}", session->get_handle(),
              session->get_remote_addr());

  handle_established(session);
  if (listener_ != nullptr)
  {
    listener_->on_established(session);
  }
}

template <typename Protocol>
void server<Protocol>::on_closed(session_ptr session, error_code ec)
{
  // remove
  {
    std::unique_lock<shared_mutex> guard(mutex_);
    sessions_.erase(session->get_handle());
  }

  LOG()->info("server session closed. handle: {}  remote: {} error: {}", session->get_handle(),
              session->get_remote_addr(), ec.message());

  handle_closed(session, ec);
  if (listener_ != nullptr)
  {
    listener_->on_closed(session, ec);
  }
}

template <typename Protocol>
void server<Protocol>::on_receive(session_ptr session, topic topic, const void* data, size_t len)
{
  if (listener_ != nullptr)
  {
    listener_->on_receive(session, topic, data, len);
  }
  handle_receive(session, topic, data, len);
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
void server<Protocol>::start_accept()
{
  auto se = std::make_shared<session>(*this, runner_.get_ioc(), true);

  acceptor_->async_accept(se->get_socket(),
                          [this, se](error_code ec)
                          {
                            handle_accept(se, ec);
                          });
}

template <typename Protocol>
void server<Protocol>::handle_accept(session_ptr se, error_code ec)
{
  if (!ec)
  {
    // add
    {
      std::unique_lock<shared_mutex> guard(mutex_);
      sessions_.insert(std::pair(se->get_handle(), se));
    }

    se->start();

    LOG()->info("session accepted. handle: {} remote: {}", se->get_handle(), se->get_remote_addr());
  }
  else
  {
    LOG()->info("accept error: {}", ec.message());
  }

  start_accept();
}

}  // namespace play