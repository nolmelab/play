#include <doctest/doctest.h>
#include <play/robust/net/protocol.hpp>
#include <play/robust/net/util/serializer.hpp>

using namespace play::robust::base;
using namespace play::robust::net;

namespace {
void func(int v) {}

struct functor
{
  virtual void f(int v) {}
};

struct f2 : public functor
{
  void f(int v) override {}
};

}  // namespace

TEST_CASE("protocol")
{
  SUBCASE("serialize")
  {
    uint8_t buf[4] = {};

    serializer::serialize<uint32_t>((uint8_t*)buf, 4, 500);

    uint32_t v;
    serializer::deserialize<uint32_t>((const uint8_t*)buf, 4, v);
    CHECK(v == 500);
  }
}

TEST_CASE("ideation verificaiton")
{
  const int test_count = 100;

  SUBCASE("adapter performance")
  {
    SUBCASE("std::function")
    {
      std::function<void(int)> f = [](int v)
      {
      };

      auto start = std::chrono::steady_clock::now();

      for (int i = 0; i < test_count; ++i)
      {
        f(i);
      }

      auto end = std::chrono::steady_clock::now();
      auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

      LOG()->info("std::function call. elapsed: {}", elapsed);
      // 1억번 호출에 132ms 걸림. 성능에 영향을 줄 요소가 아님
    }

    SUBCASE("function")
    {
      auto start = std::chrono::steady_clock::now();

      for (int i = 0; i < test_count; ++i)
      {
        func(i);
      }

      auto end = std::chrono::steady_clock::now();
      auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

      LOG()->info("func call. elapsed: {}", elapsed);
      // func() 호출이 아주 훨씬 빠르기는 함.
      //
    }

    // 둘 간의 성능 차이는 수만배 이상이다. 하지만 네트워크 호출은 초당 수백만 호출이므로
    // 충부한 성능을 제공할 수 있고 거의 0에 가까운 부하이기 때문에
    // 구조를 분리할 수 있는 기능이 강력하여 protocol::adapter를 사용한다.

    SUBCASE("virtual ")
    {
      auto start = std::chrono::steady_clock::now();

      f2 f;

      for (int i = 0; i < test_count; ++i)
      {
        f.f(i);
      }

      auto end = std::chrono::steady_clock::now();
      auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

      LOG()->info("virtual func call. elapsed: {}", elapsed);
      // 가상 함수 호출도 매우 빠르다.
    }
  }
}

// note: function2
// - 개선된 std::function이라고 하여 간단한 성능 테스트를 진행
// - std::function보다 약간 느리다. inline, 다른 최적화가 안 돼서 그런 듯
