#pragma once

#include <deque>
#include <memory>
#include <mutex>
#include <shared_mutex>

namespace play {

// std::shared_ptr에 deleter를 추가하여 풀로 반환하게 한 간단한 풀
template <typename T, typename Mutex = std::recursive_mutex>
class object_pool
{
public:
  struct deleter
  {
    deleter(object_pool* pool)
        : pool_{pool}
    {
    }
    void operator()(const T* obj)
    {
      obj->~T();
      pool_->release(const_cast<T*>(obj));
    }
    object_pool* pool_;
  };

public:
  ~object_pool()
  {
    for (auto& p : pool_)
    {
      delete p;
    }
  }
  template <typename... Args>
  std::shared_ptr<T> construct(Args... args)
  {
    std::lock_guard<Mutex> guard(lock_);
    alloc_count_++;
    if (pool_.empty())
    {
      return std::shared_ptr<T>(new T(std::forward<Args>(args)...), deleter(this));
    }

    auto p = pool_.front();
    auto rp = std::shared_ptr<T>(new (p) T(std::forward<Args>(args)...), deleter(this));
    pool_.pop_front();
    return rp;
  }

  void release(T* p)
  {
    std::lock_guard<Mutex> guard(lock_);
    free_count_++;
    pool_.push_back(p);
    pool_size_ = pool_.size();
  }

  size_t get_alloc_count() const
  {
    return alloc_count_;
  }
  size_t get_free_count() const
  {
    return free_count_;
  }
  size_t get_pool_size() const
  {
    return pool_size_;
  }

private:
  std::deque<T*> pool_;
  Mutex lock_;
  size_t alloc_count_{0};
  size_t free_count_{0};
  size_t pool_size_{0};
};

}  // namespace play