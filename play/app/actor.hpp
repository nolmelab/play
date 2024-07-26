#pragma once

#include <map>
#include <play/app/act.hpp>
#include <play/base/dynamic_snowflake.hpp>
#include <typeindex>

namespace play { namespace app {

// actor들의 기반 클래스
/**
 * actor는 전용 strand나 전용 쓰레드에서 실행
 * actor는 act들을 가지며 act들에서 구체적인 행동을 구현
 */
class actor : public std::enable_shared_from_this<actor>
{
public:
  using ptr = std::shared_ptr<actor>;

public:
  actor();

  ~actor() {}

  // 시작
  virtual bool start() = 0;

  // 종료
  virtual void stop() = 0;

  // 동적으로 생성한 고유 아이디를 제공
  size_t get_id() const
  {
    return id_;
  }

  template <typename Act, typename... Args>
  bool create_act(Args&&... args);

  template <typename Act>
  std::shared_ptr<Act> get_act();

  template <typename Act>
  bool has_act();

protected:
  virtual void on_created_act(const std::type_index& type_index, act::ptr ap);

private:
  using act_map = std::map<std::type_index, act::ptr>;

private:
  act_map acts_;
  size_t id_;
};

}}  // namespace play::app

#include <play/app/actor.ipp>