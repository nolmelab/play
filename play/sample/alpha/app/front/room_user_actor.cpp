#include <alpha/app/front/room_actor.hpp>
#include <alpha/app/front/room_user_actor.hpp>

namespace alpha {

bool room_user_actor::on_start()
{
  pulse_ = std::make_unique<app::pulse>();
  pulse_->as_child(room_.get_pulse_front())
      .with_strand(room_.get_id())  // room이 단일 쓰레드 문맥
      .with_session(session_)
      .sub(alpha::topic::room_req_chat, PULSE_FN(on_req_chat))
      .sub(alpha::topic::room_req_leave, PULSE_FN(on_req_leave))
      .sub(app::pulse::topic_closed, PULSE_FN(on_closed))
      .start();

  return true;
}

void room_user_actor::on_stop()
{
  pulse_.reset();
}

void room_user_actor::on_req_chat(app::pulse::session_ptr se, app::pulse::frame_ptr fr)
{
  auto req = std::static_pointer_cast<room::req_chatT>(fr);

  room::res_chatT res;
  res.ec = error_code::success;
  pulse_->send<room::res_chat>(topic::room_res_chat, res);

  room_.do_chat(*req);
}

void room_user_actor::on_req_leave(app::pulse::session_ptr se, app::pulse::frame_ptr fr)
{
  send_syn_leave_f2b();
  room_.post_del_user(get_id());

  room::res_leaveT res;
  res.user_name = get_name();
  res.room = std::make_unique<room::room_infoT>();
  fill_room_info(*res.room);

  pulse_->send<room::res_leave>(topic::room_res_leave, res);
  room_.get_pulse_back()->send<room::res_leave>(topic::room_res_leave, res);
}

void room_user_actor::on_closed(app::pulse::session_ptr se, app::pulse::frame_ptr fr)
{
  send_syn_leave_f2b();
  room_.post_del_user(get_id());
}

void room_user_actor::send_syn_leave_f2b()
{
  room::res_leaveT res;
  res.user_name = name_;
  res.room = std::make_unique<room::room_infoT>();
  fill_room_info(*res.room);

  room_.get_pulse_back()->send<room::res_leave>(topic::room_syn_leave_f2b, res);
}

void room_user_actor::fill_room_info(room::room_infoT& info)
{
  info.name = room_.get_name();
  info.uuid = room_.get_uuid_string();
  info.capacity = room_.get_capacity();
  info.current = room_.get_users().size();
}

}  // namespace alpha