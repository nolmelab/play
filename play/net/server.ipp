#include <play/net/server.hpp>

namespace play {

template <typename Protocol, typename Frame>
server<Protocol, Frame>::server(runner& runner, frame_handler& handler)
    : runner_{runner},
      frame_handler_{handler}
{
}

template <typename Protocol, typename Frame>
server<Protocol, Frame>::server(runner& runner)
    : runner_{runner},
      frame_handler_{frame_subclass_handler<session, topic, Frame>::get()}
{
}

template <typename Protocol, typename Frame>
bool server<Protocol, Frame>::start(uint16_t port)
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

template <typename Protocol, typename Frame>
typename server<Protocol, Frame>::session_ptr server<Protocol, Frame>::get_session(size_t handle)
{
  std::shared_lock<shared_mutex> guard(mutex_);
  auto iter = sessions_.find(handle);
  if (iter == sessions_.end())
  {
    return {};
  }
  return iter->second;
}

template <typename Protocol, typename Frame>
template <typename FrameHandler>
FrameHandler& server<Protocol, Frame>::get_handler() const
{
  return static_cast<FrameHandler&>(frame_handler_);
}

template <typename Protocol, typename Frame>
void server<Protocol, Frame>::stop()
{
  on_stop();

  if (!!acceptor_)
  {
    acceptor_.reset();
  }

  LOG()->info("server stopped");
}

template <typename Protocol, typename Frame>
void server<Protocol, Frame>::on_established(session_ptr session)
{
  LOG()->info("server session: {} established. remote: {}", session->get_handle(),
              session->get_remote_addr());

  handle_established(session);
}

template <typename Protocol, typename Frame>
void server<Protocol, Frame>::on_closed(session_ptr session, error_code ec)
{
  // remove
  {
    std::unique_lock<shared_mutex> guard(mutex_);
    sessions_.erase(session->get_handle());
  }

  LOG()->info("server session closed. handle: {}  remote: {} error: {}", session->get_handle(),
              session->get_remote_addr(), ec.message());

  handle_closed(session, ec);
}

template <typename Protocol, typename Frame>
void server<Protocol, Frame>::on_receive(session_ptr session, topic topic, const void* data,
                                         size_t len)
{
  frame_handler_.recv(session, topic, data, len);
  handle_receive(session, topic, data, len);
}

template <typename Protocol, typename Frame>
bool server<Protocol, Frame>::on_start()
{
  return true;
}

template <typename Protocol, typename Frame>
void server<Protocol, Frame>::on_stop()
{
}

template <typename Protocol, typename Frame>
void server<Protocol, Frame>::start_accept()
{
  auto se = std::make_shared<session>(*this, runner_.get_ioc(), true);

  acceptor_->async_accept(se->get_socket(),
                          [this, se](error_code ec)
                          {
                            handle_accept(se, ec);
                          });
}

template <typename Protocol, typename Frame>
void server<Protocol, Frame>::handle_accept(session_ptr se, error_code ec)
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