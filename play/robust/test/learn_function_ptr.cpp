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
  // 디버그에서는 괜찮으나 최적화를 하면 바로 크래시
}

namespace {

template <typename Topic>
struct protocol_adapter_base
{
  void (*send_t)(const void* data, size_t len);

  void (*forward_t)(Topic topic, const void* data, size_t len);

  void (*established_t)(size_t handle);
};

using topic = uint32_t;

struct session
{
  void send(const void* data, size_t len) {}

  void forward(topic topic, const void* data, size_t len) {}

  void established(size_t handle) {}
};

struct protocol_adapter : public protocol_adapter_base<topic>
{
  protocol_adapter(session* ss_)
  {
    send_t = send_impl;
    forward = forward_impl;
    established = established_impl;
  }

  void send_impl(const void* data, size_t len) { ss_->send(data, len); }

  void forward_impl(topic topic, const void* data, size_t len) { ss_->forward(topic, data, len); }

  void established_impl(size_t handle) { ss_->established(0); }

  session* ss_;
};

}  // namespace

TEST_CASE("manual inheritance")
{
  session ss;

  protocol_adapter adapter(&ss);
}