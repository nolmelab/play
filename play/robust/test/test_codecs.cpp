#include <doctest/doctest.h>
#include <play/robust/net/protocol/length_delimited.hpp>

using namespace play::robust::net;

TEST_CASE("codecs")
{
    SUBCASE("length delimited")
    {
        length_delimited codec;

        // codec.encode();
        // codec.decode();

    }

    SUBCASE("sodium handshaker")
    {

    }

    SUBCASE("sodium cipher")
    {

    }
}