#pragma once

#include <play/ensure/act_base.hpp>

namespace alpha {

// alpha용 act의 base 클래스. 유틸리티 함수 추가
class act_base : public ensure::act_base
{
public:
  act_base(play::actor& owner, act::ptr parent, const nlohmann::json& json, const std::string& name)
      : ensure::act_base(owner, parent, json, name)
  {
  }
};

}  // namespace alpha

#define ACT_SUB_SESSION(func)

#define ACT_SUB_TOPIC(func)