#pragma once

#include <play/app/actor.hpp>
#include <play/ensure/act.hpp>
#include <play/ensure/bot.hpp>
#include <play/ensure/ensure_app.hpp>

namespace ensure {

// 앱용 ensure에서 기반 클래스로 사용. app과 bot에 접근
class act_base : public ensure::act
{
public:
  act_base(play::actor& owner, act::ptr parent, const nlohmann::json& json, const std::string& name)
      : ensure::act(owner, parent, json, name)
  {
  }

  // 형 변환으로 앱을 얻음
  ensure_app& get_app()
  {
    return static_cast<ensure_app&>(get_bot().get_app());
  }

  // 형 변환으로 봇을 얻음
  ensure::bot& get_bot()
  {
    return static_cast<ensure::bot&>(get_owner());
  }
};

}  // namespace ensure
