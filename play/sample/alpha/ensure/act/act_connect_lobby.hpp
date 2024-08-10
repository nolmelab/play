#pragma once

#include <alpha/ensure/act.hpp>

namespace alpha {

class act_connect_lobby : public act
{
public:
  act_connect_lobby(play::actor& owner, act::ptr parent, const nlohmann::json& json,
                    const std::string& name)
      : act(owner, parent, json, name)
  {
  }

private:
  bool on_activate() final;

  void on_load_json() final;

private:
  std::string address_;
};

}  // namespace alpha