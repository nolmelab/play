#pragma once

#include <memory>

namespace play { namespace robust { namespace app {

class actor;

// actor내 행동들의 기반 클래스
class act 
{
public:
  using ptr = std::shared_ptr<act>;

public:
  act(actor& owner, std::string_view type_name)
      : owner_{owner}
  {
  }

  virtual bool activate() = 0;

  virtual void deactivate() = 0;

  virtual void destroy() = 0;

  actor& get_owner()
  {
    return owner_;
  }

  const actor& get_owner() const
  {
    return owner_;
  }

private:
  actor& owner_;
};

}}}  // namespace play::robust::app