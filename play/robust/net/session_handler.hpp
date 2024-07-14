#pragma once

#include <memory>
#include <play/robust/net/asio.hpp>

namespace play { namespace robust { namespace net {

template <typename Protocol>
class session;

template <typename Protocol>
class session_handler
{
public:
  using session_ptr = std::shared_ptr<session<Protocol>>;

public:
  virtual void on_established(session_ptr session) = 0;
  virtual void on_closed(session_ptr session, boost::system::error_code ec) = 0;
  virtual void on_receive(session_ptr session, typename Protocol::topic topic, const void* data,
                          size_t len) = 0;
};

}}}  // namespace play::robust::net