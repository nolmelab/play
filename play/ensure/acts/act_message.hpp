#pragma once

#include <play/ensure/act.hpp>

namespace ensure {

class act_message : public act
{
public:
  act_message(play::actor& owner, act::ptr parent, const nlohmann::json& json,
              const std::string& name);

private:
  bool on_activate() final;

  void on_update() final;
};

}  // namespace ensure