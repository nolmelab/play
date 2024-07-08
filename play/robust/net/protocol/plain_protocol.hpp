#pragma once

#include <play/robust/net/protocol/protocol.hpp>

namespace play { namespace robust { namespace net {

template <typename Topic, typename Frame>
class plain_protocol : public protocol
{
public:
    using send_fn = std::function<void(const char* data, size_t len)>;

public:
    plain_protocol(send_fn send)
    : protocol(send)
    {}

    void receive(const char* data, size_t len);

private: 
};

}}} // play::robust::net