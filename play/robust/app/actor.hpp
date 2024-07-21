#pragma once

#include <map>
#include <play/robust/app/act.hpp>
#include <play/robust/app/object.hpp>
#include <play/robust/base/dynamic_snowflake.hpp>

namespace play { namespace robust { namespace app {

// actor들의 기반 클래스
/**
 * actor는 전용 strand나 전용 쓰레드에서 실행
 * actor는 act들을 가지며 act들에서 구체적인 행동을 구현
 */
class actor : public std::enable_shared_from_this<actor>
{
public:
  actor() {}

  ~actor() {}

  // 활성화 하여 시작할 준비
  virtual bool activate() = 0;

  // 비활성화 하여 쉬도록 함
  virtual void deactivate() = 0;

  // 완전한 종료 처리
  virtual void destroy() = 0;

  template <typename Act, typename... Args>
  bool create_act(Args&&... args);

  template <typename Act>
  std::shared_ptr<Act> get_act();

private:
  using act_map = std::map<std::type_index, act::ptr>;

private:
  act_map acts_;
};

}}}  // namespace play::robust::app

#include <play/robust/app/actor.ipp>