#include <doctest/doctest.h>
#include <play/base/observer_ptr.hpp>

TEST_CASE("observer_ptr")
{
  SUBCASE("usage")
  {
    int v = 3;
    play::observer_ptr<int> p{&v};
    CHECK(*p == 3);

    // non-owning 포인터로 표준에 제출되었으나 채택되지는 않은 상태.
    // 필요한 곳에 써볼 수 있을 듯 하여 play namespace에 포함
  }
}