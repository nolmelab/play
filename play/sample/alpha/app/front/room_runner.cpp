#include <alpha/app/front/room_runner.hpp>

namespace alpha {

bool room_runner::on_start()
{
  pulse_front_ = std::make_unique<app::pulse>();
  pulse_front_->as_child(&get_app().get_pulse())
      .with_strand(get_id())
      .sub(alpha::topic::room_req_checkin, PULSE_FN(on_room_req_checkin))
      .start();

  // established 처리를 위해 임시 등록
  pulse_back_ = std::make_unique<app::pulse>();
  pulse_back_->as_child(&get_app().get_pulse_back())
      .with_strand(get_id())
      .sub(app::pulse::topic_estalished, PULSE_FN(on_established_back))
      .start();

  LOG()->info("lobby runner started");

  return true;
}

void room_runner::on_stop()
{
  pulse_front_.reset();
  pulse_back_.reset();

  LOG()->info("lobby runner stopped");
}

void room_runner::on_established_back(app::pulse::session_ptr se, app::pulse::frame_ptr fr)
{
  // 다시 펄스를 만들면서 세션을 연결. 연결이 끊어지고 다시 맺어지면 자동 복구
  pulse_back_.reset();
  pulse_back_ = std::make_unique<app::pulse>();
  pulse_back_->as_child(&get_app().get_pulse_back())
      .with_strand(get_id())
      .with_session(se)
      .sub(app::pulse::topic_estalished, PULSE_FN(on_established_back))
      .sub(app::pulse::topic_closed, PULSE_FN(on_closed_back))
      .sub(alpha::topic::room_req_create_b2f, PULSE_FN(on_room_req_create_b2f))
      .sub(alpha::topic::room_req_reserve_b2f, PULSE_FN(on_room_req_reserve_b2f))
      .start();
}

void room_runner::on_closed_back(app::pulse::session_ptr se, app::pulse::frame_ptr fr)
{
  // 뒷단 통신에 앞단에서 시작되는 경우가 없는 경우.
}

void room_runner::on_room_req_create_b2f(app::pulse::session_ptr se, app::pulse::frame_ptr fr)
{
  auto req = std::static_pointer_cast<room::req_createT>(fr);
}

void room_runner::on_room_req_reserve_b2f(app::pulse::session_ptr se, app::pulse::frame_ptr fr)
{
  auto req = std::static_pointer_cast<room::req_reserveT>(fr);
}

void room_runner::on_room_req_checkin(app::pulse::session_ptr se, app::pulse::frame_ptr fr)
{
  auto req = std::static_pointer_cast<room::req_checkinT>(fr);
}

}  // namespace alpha