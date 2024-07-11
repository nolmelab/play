#pragma once 

#include <play/robust/net/codec.hpp>
#include <optional>

namespace play { namespace robust { namespace net {

// 페이로드 길이를 앞에 32비트 갖는 코덱이다. 
class length_delimited : public codec 
{
public: 
    length_delimited() = default;

    // get one frame from src_buf and put it into const_buffer
    /**
     * 돌려주는 버퍼는 src_buf의 내부 메모리 중 길이 필드를 제외하고 페이로드 만큼
     * 페이로드가 부족하면 std::optional의 has_value()가 false
     */
    std::optional<const_buffer> decode(const const_buffer& src_buf);

    // put length field into dest_buf with src_buf content as payload
    /**
     * dest_buf는 src_buf.size() + 4 (length field 길이) 바이트를 확보해야 함
     */
    size_t encode(const const_buffer& src_buf, mutable_buffer& dest_buf);
};

}}} // play::robust::net