#pragma once

#include <memory>

namespace play { namespace robust { namespace app {

class actor;

// actor내 행동들의 기반 클래스
/**
 * actors act. 논리적인 처리의 구조화의 핵심. 
 * 거의 비어있지만 개념적으로 중요 
 */
class act 
{
public:
  using ptr = std::shared_ptr<act>;

public:
  act(actor& owner, std::string_view type_name)
      : owner_{owner}
  {
  }

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