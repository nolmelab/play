#pragma once

#include <play/robust/net/asio.hpp>

namespace play { namespace robust { namespace net {

template <typename Protocol>
class session
{
public: 
    using ptr = std::shared_ptr<session<Protocol>>;

public: 
    session(asio::io_context& ios);

private:
    asio::io_context& ios_;
};

}}} // play::robust::net

// template member function implementation
#include <play/robust/net/protocol/session.ipp>