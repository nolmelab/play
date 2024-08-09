#pragma once

#include <alpha/ensure/act/client.hpp>
#include <alpha/ensure/act_base.hpp>

namespace alpha {

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

  void on_session_state(session_ptr session, frame_handler::session_state state);

private:
  std::string address_;
};

}  // namespace alpha