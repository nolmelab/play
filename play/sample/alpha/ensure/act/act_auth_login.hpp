#pragma once

#include <alpha/ensure/act_base.hpp>

namespace alpha {

// 로그인 처리
class act_auth_login : public act_base
{
public:
  act_auth_login(play::actor& owner, act::ptr parent, const nlohmann::json& json,
                 const std::string& name)
      : act_base(owner, parent, json, name)
  {
  }

private:
  bool on_activate() final;

  void on_deactivate() final;

  bool req_login();

  void on_res_login(session_ptr se, frame_ptr fr);

private:
  size_t sub_id_{0};
};

}  // namespace alpha