#include <doctest/doctest.h>

#include <chrono>
#include <play/robust/base/logger.hpp>
#include <type_traits>

namespace {
template <typename>
struct TransientFunction;  // intentionally not defined

template <typename R, typename... Args>
struct TransientFunction<R(Args...)>
{
  using Dispatcher = R (*)(void*, Args...);

  Dispatcher m_Dispatcher;  // A pointer to the static function that will call the
                            // wrapped invokable object
  void* m_Target;           // A pointer to the invokable object

  // Dispatch() is instantiated by the TransientFunction constructor,
  // which will store a pointer to the function in m_Dispatcher.
  template <typename S>
  static R Dispatch(void* target, Args... args)
  {
    return (*(S*)target)(args...);
  }

  template <typename T>
  TransientFunction(T&& target)
      : m_Dispatcher(&Dispatch<typename std::decay<T>::type>), m_Target(&target)
  {
  }

  // Specialize for reference-to-function, to ensure that a valid pointer is
  // stored.
  using TargetFunctionRef = R(Args...);
  TransientFunction(TargetFunctionRef target) : m_Dispatcher(Dispatch<TargetFunctionRef>)
  {
    static_assert(sizeof(void*) == sizeof target,
                  "It will not be possible to pass functions by reference on this platform. "
                  "Please use explicit function pointers i.e. foo(target) -> foo(&target)");
    m_Target = (void*)target;
  }

  R operator()(Args... args) const { return m_Dispatcher(m_Target, args...); }
};

}  // namespace

TEST_CASE("performance")
{
  const int test_count = 100000000;

  SUBCASE("call")
  {
    auto start = std::chrono::steady_clock::now();

    int k = 0;

    TransientFunction<void(int)> f = [](int v)
    {
    };

    for (int i = 0; i < test_count; ++i)
    {
      f(i);
    }

    auto end = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    LOG()->info("transient func call. elapsed: {}", elapsed);
  }

  // 아쉽게도 capture를 한 람다에 대해서는 SIGSEGV가 발생
  // 이와 같은 기능이 생각보다 구현이 어렵다는 걸 보여줌.
}