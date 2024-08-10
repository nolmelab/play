#pragma once

#include <map>
#include <memory>
#include <string>

namespace play {

// 초간단 블랙보드
/**
 * 간단한 사용법:
 * @code
 * bot::get_blackboard().set("id", std::make_shared<mem_obj<std::string>>("hello_user_1"));
 * auto id = bot::get_blackboard().get<mem_obj<std::string>>("id")->value();
 * @endcode 
 * 
 * bot::get_blackboard().set("client", std::make_shared<mem_ptr<net::client>(client));
 * bot::get_blackboard().get<mem_ptr<net::client>>("client")->value()->close();
 */
class blackboard
{
public:
  // erase types of memory and use them with type casts where programmers know the types
  class mem
  {
  public:
    using ptr = std::shared_ptr<mem>;

  public:
    virtual ~mem() {}
  };

public:
  blackboard() = default;

  void set(const std::string& key, mem::ptr m)
  {
    memory_.insert(std::pair{key, m});
  }

  mem::ptr get(const std::string& key)
  {
    auto iter = memory_.find(key);
    if (iter == memory_.end())
    {
      return {};
    }
    return iter->second;
  }

  template <typename T>
  std::shared_ptr<T> get(const std::string& key)
  {
    auto iter = memory_.find(key);
    if (iter == memory_.end())
    {
      return {};
    }
    return std::static_pointer_cast<T>(iter->second);
  }

private:
  using mem_map = std::map<std::string, mem::ptr>;

private:
  mem_map memory_;
};

// 복사 가능한 임의의 오브젝트를 저장
template <typename Obj>
class mem_obj : public blackboard::mem
{
public:
  mem_obj(Obj p)
      : p_{p}
  {
  }

  const Obj& value() const
  {
    return p_;
  }

private:
  Obj p_;
};

template <typename T>
class mem_ptr : public blackboard::mem
{
public:
  using ptr = std::shared_ptr<T>;

public:
  mem_ptr(ptr p)
      : p_{p}
  {
  }

  ptr value()
  {
    return p_;
  }

private:
  ptr p_;
};

}  // namespace play