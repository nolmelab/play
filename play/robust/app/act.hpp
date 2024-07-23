#pragma once

#include <memory>

namespace play { namespace robust { namespace app {

class actor;

// actor내 행동들의 기반 클래스
/**
 * actors act. 논리적인 처리의 구조화의 핵심. 
 * 비어 있으므로 개별 앱에서 필요한 구조로 사용
 * 
 * @see act_ensure
 */
class act 
{
public:
  using ptr = std::shared_ptr<act>;

public:
  act(actor& owner)
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