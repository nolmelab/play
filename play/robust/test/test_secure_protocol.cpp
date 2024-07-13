#include <doctest/doctest.h>
#include <play/robust/base/logger.hpp>
#include <play/robust/net/protocol/bytes_frame.hpp>
#include <play/robust/net/protocol/secure_protocol.hpp>

using namespace play::robust::net;

TEST_CASE("secure_protocol")
{
  SUBCASE("basic communication")
  {
    // 자체 테스트를 하면 좋으나 걸리는 부분이 많아 통신을 구현 후 진행
    // plain_protocol을 구현하고 진행
    secure_protocol<uint32_t>::adapter adapter;
    secure_protocol<uint32_t> p(1, adapter);
  }
}