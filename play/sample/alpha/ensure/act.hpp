#pragma once

#include <alpha/ensure/ensure.hpp>
#include <play/ensure/act_ensure.hpp>
#include <play/ensure/bot.hpp>

namespace alpha {

// alpha용 act의 base 클래스. 유틸리티 함수 추가
class act : public play::act_ensure
{
public:
  act(play::actor& owner, act_ensure::ptr parent, const nlohmann::json& json,
      const std::string& name)
      : play::act_ensure(owner, parent, json, name)
  {
  }

  ~act() {};
};

}  // namespace alpha
