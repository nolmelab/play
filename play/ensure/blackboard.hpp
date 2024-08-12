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

  // 복사 가능한 임의의 오브젝트를 저장
  template <typename Obj>
  class mem_obj : public mem
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
  class mem_ptr : public mem
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

public:
  blackboard() = default;

  void set(const std::string& key, mem::ptr m)
  {
    memory_.insert(std::pair{key, m});
  }

  template <typename T>
  void set_obj(const std::string& key, T&& v)
  {
    set(key, std::make_shared<mem_obj<T>>(std::forward<T>(v)));
  }

  template <typename T>
  void set_ptr(const std::string& key, std::shared_ptr<T> p)
  {
    set(key, std::make_shared<mem_ptr<T>>(p));
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

  template <typename Obj>
  const Obj& get_obj(const std::string& key)
  {
    auto iter = memory_.find(key);
    if (iter == memory_.end())
    {
      return {};
    }
    auto p = std::static_pointer_cast<blackboard::mem_obj<Obj>>(iter->second);
    return p->value();
  }

  template <typename T>
  std::shared_ptr<T> get_ptr(const std::string& key)
  {
    auto iter = memory_.find(key);
    if (iter == memory_.end())
    {
      return {};
    }
    auto p = std::static_pointer_cast<blackboard::mem_ptr<T>>(iter->second);
    return p->value();
  }

private:
  using mem_map = std::map<std::string, mem::ptr>;

private:
  mem_map memory_;
};

}  // namespace play