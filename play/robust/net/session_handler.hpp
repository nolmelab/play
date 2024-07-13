#pragma once

#include <boost/asio.hpp>

namespace play { namespace robust { namespace net {

template <typename Protocol>
class session_handler
{
public:
  using session = session<Protocol>;

protected:
  virtual void on_connected(session::ptr session) = 0;
  virtual void on_accepted(session::ptr session) = 0;
  virtual void on_close(session::ptr session, boost::asio::error_code ec) = 0;
};

}}}  // namespace play::robust::net