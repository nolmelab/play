#pragma once

#include <alpha/ensure/act_base.hpp>

namespace ensure {

class act_connect_lobby : public act_base
{
public:
  act_connect_lobby(play::actor& owner, act::ptr parent, const nlohmann::json& json,
                    const std::string& name)
      : act_base(owner, parent, json, name)
  {
  }

private:
  bool on_activate() final;

  void on_load_json() final;
};

}  // namespace ensure