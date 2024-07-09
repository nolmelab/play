#pragma once

#include <play/robust/net/protocol/protocol.hpp>

namespace play { namespace robust { namespace net {

// length delimited and sodium_cipher optional topic field protocol
template <typename Topic, typename Frame>
class secure_protocol : public protocol<Topic, Frame>
{
public:
    secure_protocol(send_fn send)
    : protocol(send)
    {}

    void on_accepted();

    void on_connected();

    void on_closed();

    void on_receive(const char* data, size_t len);

private: 
    // recv buffer
    // working buffer
};

}}} // play::robust::net

// template member function implementation
#include <play/robust/net/protocol/secure_protocol.ipp>