#pragma once

#include <play/ensure/act_ensure.hpp>

namespace play {

class act_message : public act_ensure
{
public:
  act_message(play::actor& owner, act_ensure::ptr parent, const nlohmann::json& json,
              const std::string& name);

private:
  bool on_activate() final;

  void on_update() final;
};

}  // namespace play