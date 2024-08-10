#include <alpha/share/fb/auth_generated.h>
#include <alpha/share/fb/topic_generated.h>
#include <alpha/ensure/act/act_auth_login.hpp>
#include <play/base/json_reader.hpp>

namespace alpha {

bool act_auth_login::on_activate()
{
  auto pulse_lobby = get_bot().get_blackboard().get_ptr<ensure::pulse>("lobby");

  pulse_ = std::make_unique<ensure::pulse>();
  pulse_->as_child(pulse_lobby.get())
      .with_strand(get_owner().get_id())
      .subscribe(topic::auth_res_login,
                 [this](ensure::pulse::session_ptr se, ensure::pulse::frame_ptr frame)
                 {

                 })
      .start();

  // pulse_lobby에 구독하고 해지하는 방식으로도 가능

  return true;
}

void act_auth_login::on_deactivate()
{
  pulse_.reset();
}

}  // namespace alpha