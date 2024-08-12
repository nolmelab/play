#include <alpha/share/fb/topic_generated.h>
#include <alpha/app/front/actor_user.hpp>
#include <alpha/app/front/lobby_runner.hpp>

namespace alpha {

bool lobby_runner::start()
{
  pulse_ = std::make_unique<app::pulse>();
  pulse_->as_child(&get_app().get_pulse())
      .with_strand(get_id())
      .subscribe(ALPHA_PULSE_CB(auth_req_login))
      .start();
  return true;
}

void lobby_runner::stop()
{
  pulse_.reset();
}

void lobby_runner::on_auth_req_login(app::pulse::session_ptr se, app::pulse::frame_ptr req)
{
  //
}

}  // namespace alpha