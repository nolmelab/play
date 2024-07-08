#include <doctest/doctest.h>
#include <play/robust/base/logger.hpp>
#include <play/robust/net/protocol/bytes_frame.hpp>
#include <play/robust/net/protocol/plain_protocol.hpp>
#include <play/robust/net/protocol/secure_protocol.hpp>

using namespace play::robust::net;

TEST_CASE("protocols")
{
    SUBCASE("plain protocol")
    {
        // test the design is on a right track 
        SUBCASE("uint16_t topic, bytes_frame")
        {
            plain_protocol<uint16_t, bytes_frame> protocol(
                [](const void* data, size_t len) {
                    // LOG()->info() // log the bytes
                }
            );
        }
    }
}