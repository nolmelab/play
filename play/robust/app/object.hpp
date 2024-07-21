#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <typeindex>
#include <typeinfo>

namespace play { namespace robust { namespace app {

//
class object_base
{
public:
  virtual ~object_base() {}
};

// base class of all app objects except app
/**
 * 오브젝트 생성 / 소멸 개수 추적에 사용 
 * 다른 공통 베이스 기능을 추가 
 */
template <typename Obj>
class object : public object_base
{
public:
  object(std::string_view type_name)
      : type_name_{type_name}
  {
    ++count_;
  }

  ~object()
  {
    --count_;
  }

private:
  static inline size_t count_{0};
  std::string type_name_;
};

}}}  // namespace play::robust::app