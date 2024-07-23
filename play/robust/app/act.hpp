#pragma once

#include <memory>

namespace play { namespace robust { namespace app {

class actor;

// actor내 행동들의 기반 클래스
<<<<<<< HEAD
/**
 * actors act. 논리적인 처리의 구조화의 핵심. 
 * 거의 비어있지만 개념적으로 중요 
 */
class act 
=======
class act
>>>>>>> b61dcf63870f5463151f8d2f3f37818ae3cf2d71
{
public:
  using ptr = std::shared_ptr<act>;

public:
  act(actor& owner)
      : owner_{owner}
  {
  }

<<<<<<< HEAD
=======
  // 활성
  virtual bool activate() = 0;

  // 비활성
  virtual void deactivate() = 0;

>>>>>>> b61dcf63870f5463151f8d2f3f37818ae3cf2d71
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