#pragma once

#include <functional>

namespace play { namespace robust { namespace net {

// type used to mark empty topic (topic not used)
struct empty_topic {};

class protocol
{
public:
    using send_fn = std::function<void(const void* data, size_t len)>;

    protocol(send_fn send_fn)
    : send_fn_{send_fn}
    {}

protected:
    void send(const void* data, size_t len)
    {
        send_fn_(data, len);
    }

private:
    send_fn send_fn_;
};

}}} // play::robust::net