#include <alpha/app/front/room_actor.hpp>
#include <alpha/app/front/room_user_actor.hpp>

namespace alpha {

bool room_user_actor::on_start()
{
  pulse_ = std::make_unique<app::pulse>();
  pulse_->as_child(room_.get_pulse_front())
      .with_strand(get_id())
      .with_session(session_)
      .sub(alpha::topic::room_req_chat, PULSE_FN(on_req_chat))
      .sub(app::pulse::topic_closed, PULSE_FN(on_closed))
      .start();

  return true;
}

void room_user_actor::on_stop()
{
  // close session?
}

void room_user_actor::on_req_chat(app::pulse::session_ptr se, app::pulse::frame_ptr req)
{
  //
}

void room_user_actor::on_closed(app::pulse::session_ptr se, app::pulse::frame_ptr req)
{
  //
}

}  // namespace alpha