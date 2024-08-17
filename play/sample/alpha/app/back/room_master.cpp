#include <alpha/share/fb/error_code_generated.h>
#include <alpha/share/fb/room_generated.h>
#include <alpha/share/fb/topic_generated.h>
#include <alpha/app/back/room_master.hpp>

namespace alpha {

bool room_master::on_start()
{
  pulse_ = std::make_unique<app::pulse>();
  pulse_->as_child(&get_app().get_pulse())
      .with_strand(get_id())
      .sub(app::pulse::topic_estalished, PULSE_FN(on_front_established))
      .sub(app::pulse::topic_closed, PULSE_FN(on_front_closed))
      .sub(topic::room_syn_runner_up, PULSE_FN(on_room_syn_runner_up))
      .sub(topic::room_req_create_f2b, PULSE_FN(on_room_req_create_f2b))
      .sub(topic::room_res_create_f2b, PULSE_FN(on_room_res_create_f2b))
      .sub(topic::room_req_page_f2b, PULSE_FN(on_room_req_page_f2b))
      .sub(topic::room_req_reserve_f2b, PULSE_FN(on_room_req_reserve_f2b))
      .sub(topic::room_res_reserve_f2b, PULSE_FN(on_room_res_reserve_f2b))
      .start();

  return true;
}

void room_master::on_stop()
{
  pulse_.reset();
}

void room_master::on_room_syn_runner_up(app::pulse::session_ptr se, app::pulse::frame_ptr fr)
{
  auto proxy = std::make_shared<room_runner_proxy>(se->get_remote_addr(), se);
  proxy->start();
  room_runners_.add(proxy);
}

void room_master::on_room_req_create_f2b(app::pulse::session_ptr se, app::pulse::frame_ptr fr)
{
  // 방 개수가 가장 적은 room_runner_proxy를 찾아서 생성 요청
}

void room_master::on_room_res_create_f2b(app::pulse::session_ptr se, app::pulse::frame_ptr fr)
{
  // user_master 서비스에 post()하여 전송
}

void room_master::on_room_req_page_f2b(app::pulse::session_ptr se, app::pulse::frame_ptr fr)
{
  //
}

void room_master::on_room_req_reserve_f2b(app::pulse::session_ptr se, app::pulse::frame_ptr fr)
{
  //
}

void room_master::on_room_res_reserve_f2b(app::pulse::session_ptr se, app::pulse::frame_ptr fr)
{
  //
}

void room_master::on_room_syn_leave_f2b(app::pulse::session_ptr se, app::pulse::frame_ptr fr)
{
  //
}

void room_master::on_front_established(app::pulse::session_ptr se, app::pulse::frame_ptr fr)
{
  //
}

void room_master::on_front_closed(app::pulse::session_ptr se, app::pulse::frame_ptr fr)
{
  // 세션으로 room_runner_proxy 삭제
}

}  // namespace alpha