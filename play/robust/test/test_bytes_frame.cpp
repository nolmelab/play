#include <doctest/doctest.h>

#include <play/robust/net/protocol/bytes_frame.hpp>
#include <array>
#include <memory>

using bytes_frame = play::robust::net::bytes_frame;

TEST_CASE("bytes_frame")
{
    SUBCASE("simple usage")
    {
        SUBCASE("same type")
        {
            std::array<uint8_t, 8> bytes{1, 2, 3, 4, 5, 6, 8};

            auto frame = std::make_shared<bytes_frame>(
                8,
                bytes.data()
            );

            CHECK(frame->size() == bytes.size());
            CHECK(frame->data()[2] == bytes[2]);
        }

        SUBCASE("different type")
        {
            std::array<char, 8> bytes{1, 2, 3, 4, 5, 6, 8};

            auto frame = std::make_shared<bytes_frame>(
                8,
                reinterpret_cast<const uint8_t*>(bytes.data())
            );

            CHECK(frame->size() == bytes.size());
            CHECK(frame->data()[2] == bytes[2]);
        }
    }

}