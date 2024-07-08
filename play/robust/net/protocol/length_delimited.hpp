#pragma once 

#include <play/robust/net/protocol/codec.hpp>

namespace play { namespace robust { namespace net {

class length_delimited : public codec 
{
public: 
    length_delimited(int length_field_size = 2)
    : length_field_size_{length_field_size}
    {}

    // get one frame from src_buf and put it into const_buffer
    const_buffer decode(const const_buffer& src_buf);

    // put length field into dest_buf with src_buf content as payload
    size_t encode(const const_buffer& src_buf, mutable_buffer& dest_buf);

private:
    int length_field_size_;
};

}}} // play::robust::net