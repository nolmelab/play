#pragma once

#include <vector>

namespace play { namespace robust { namespace net { 

/// bytes_frame holding bytes 
/** 
 * @code
 *  auto frame = std::make_shared<bytes_frame>(len, data);
 * @endcode
 */
class bytes_frame 
{
public: 
    bytes_frame(size_t size, const uint8_t* bytes)
    : size_(size)
    {
        buf_.resize(size_);
        memcpy(buf_.data(), bytes, size_);
    }

    size_t size() const 
    {
        return size_;
    }

    const uint8_t* data() const 
    {
        return buf_.data();
    }

private:
    size_t size_;
    std::vector<uint8_t> buf_;
};

}}} // play::robust::net