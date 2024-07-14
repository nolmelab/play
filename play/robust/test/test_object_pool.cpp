#include <doctest/doctest.h>
#include <chrono>
#include <deque>
#include <memory>
#include <play/robust/base/logger.hpp>

#include <play/robust/base/object_pool.hpp>

using namespace play::robust::base;

namespace {
struct simple
{
  simple(int v) : v_{v} {}
  simple(int v, float f) : v_{v} {}
  ~simple() { /* LOG()->info("simple destructor called"); */ }

  int v_;
};
}  // namespace

TEST_CASE("object_pool")
{
  SUBCASE("basic")
  {
    object_pool<simple> pool;

    const int test_count = 1000000;

    auto start = std::chrono::steady_clock::now();

    CHECK(pool.get_pool_size() == 0);

    for (int i = 0; i < test_count; ++i)
    {
      auto sp = pool.construct(1);
    }

    CHECK(pool.get_alloc_count() == test_count);
    CHECK(pool.get_free_count() == test_count);
    CHECK(pool.get_pool_size() == 1);

    auto end = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    LOG()->info("pool. elapsed: {}", elapsed);
  }
}

// 아래는 아이디어 테스트 코드

namespace {

template <typename T>
class pool
{
public:
  struct deleter
  {
    deleter(pool* p) : p_{p} {}
    void operator()(const T* obj) { p_->release(const_cast<T*>(obj)); }
    pool* p_;
  };

public:
  template <typename... Args>
  pool(size_t initial_count, Args... args)
  {
    reserve(initial_count, std::forward<Args>(args)...);
  }

  template <typename... Args>
  std::shared_ptr<T> make_shared(Args... args)
  {
    if (pool_.empty())
    {
      return std::shared_ptr<T>(new T(std::forward<Args>(args)...), deleter(this));
    }

    auto p = pool_.front();
    auto rp = std::shared_ptr<T>(new (p) T(std::forward<Args>(args)...), deleter(this));
    pool_.pop_front();
    return rp;
  }

  void release(T* p) { pool_.push_back(p); }

private:
  template <typename... Args>
  void reserve(size_t count, Args... args)
  {
    for (size_t i = 0; i < count; ++i)
    {
      pool_.push_back(new T(std::forward<Args>(args)...));
    }
  }

private:
  std::deque<T*> pool_;
};

}  // namespace

TEST_CASE("idea")
{
  //
  pool<simple> p(128, 1);

  {
    auto s1 = p.make_shared(1);
    CHECK(s1->v_ == 1);
  }
}
