#pragma once

#include <alpha/ensure/act.hpp>
#include <alpha/ensure/ensure.hpp>

namespace alpha {

// 로그인 처리
class act_auth_login : public act
{
public:
  act_auth_login(play::actor& owner, act::ptr parent, const nlohmann::json& json,
                 const std::string& name)
      : act(owner, parent, json, name)
  {
  }

private:
  bool on_activate() final;

  void on_deactivate() final;

private:
  size_t sub_id_{0};
  std::unique_ptr<ensure::pulse> pulse_;
};

}  // namespace alpha