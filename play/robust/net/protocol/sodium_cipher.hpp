#pragma once 

#include <play/robust/net/protocol/codec.hpp>

namespace play { namespace robust { namespace net {

class sodium_cipher : public codec 
{
public: 
    // decrypt src_buf with internal buffer and put it into const_buffer
    const_buffer decode(const const_buffer& src_buf);

    // encrypt src_buf into dest_buf 
    size_t encode(const const_buffer& src_buf, mutable_buffer& dest_buf);
};

}}} // play::robust::net