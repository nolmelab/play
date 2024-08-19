#include <alpha/share/fb/error_code_generated.h>
#include <alpha/share/fb/room_generated.h>
#include <alpha/share/fb/topic_generated.h>
#include <alpha/app/back/room_master.hpp>
#include <alpha/app/back/room_proxy.hpp>
#include <alpha/app/back/room_runner_proxy.hpp>

namespace alpha {

bool room_proxy::on_start()
{
  pulse_ = std::make_unique<app::pulse>();
  pulse_->as_child(runner_proxy_.get_pulse())
      .with_strand(runner_proxy_.get_master().get_id())
      .with_session(session_)
      .mark_final()
      .sub(topic::room_syn_leave_f2b, PULSE_FN(on_room_syn_leave_f2b))
      .sub(topic::room_res_reserve_f2b, PULSE_FN(on_room_res_reserve_f2b))
      .start();

  return true;
}

void room_proxy::on_stop()
{
  pulse_.reset();
}

void room_proxy::on_room_syn_leave_f2b(app::pulse::session_ptr se, app::pulse::frame_ptr fr)
{
  // TODO
}

void room_proxy::on_room_res_reserve_f2b(app::pulse::session_ptr se, app::pulse::frame_ptr fr)
{
  // TODO
}

}  // namespace alpha