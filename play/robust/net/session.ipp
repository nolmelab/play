#include <play/robust/net/protocol/session.hpp>

namespace play { namespace robust { namespace net {

template <typename Protocol>
session<Protocol>::session(boost::asio::io_context& ios)
: ios_(ios)
{
}


}}} // play::robust::net