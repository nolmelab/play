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
      .sub(alpha::topic::room_req_create_b2f, PULSE_FN(on_room_req_create_b2f))
      .sub(alpha::topic::room_req_reserve_b2f, PULSE_FN(on_room_req_reserve_b2f))
      .start();
}

void room_runner::on_room_req_create_b2f(app::pulse::session_ptr se, app::pulse::frame_ptr fr)
{
  auto req = std::static_pointer_cast<room::req_createT>(fr);

  // 방 생성 요청이 뒷단을 거쳐 옴
  auto ra =
      std::make_shared<room_actor>(*this, req->room->name, req->room->capacity, req->user_name);
  ra->start();
  rooms_.add(ra);

  room::res_createT res;
  res.room->name = req->room->name;
  res.room->uuid = ra->get_uuid_string();
  res.room->current = 0;
  res.room->capacity = req->room->capacity;

  pulse_back_->send<room::res_create>(topic::room_res_create_f2b, res);
}

void room_runner::on_room_req_reserve_b2f(app::pulse::session_ptr se, app::pulse::frame_ptr fr)
{
  auto req = std::static_pointer_cast<room::req_reserveT>(fr);
  auto rv = rooms_.find(req->uuid);
  if (rv)
  {
    auto room = rv.value();
    room->do_reserve(*req);
  }
  else
  {
    room::res_reserveT res;
    res.ec = error_code::fail_room_not_found;
    res.user_name = req->user_name;
    res.room = std::make_unique<room::room_infoT>();
    res.room->uuid = req->uuid;

    pulse_back_->send<room::res_reserve>(topic::room_res_reserve_f2b, res);
  }
}

void room_runner::on_room_req_checkin(app::pulse::session_ptr se, app::pulse::frame_ptr fr)
{
  auto req = std::static_pointer_cast<room::req_checkinT>(fr);
  auto _uuid = play::uuid::from(req->room->uuid);
  auto rv = rooms_.find(_uuid);
  if (rv)
  {
    rv.value()->do_chekin(se, *req);
  }
  else
  {
    room::res_checkinT res;
    res.ec = error_code::fail_room_not_found;
    pulse_front_->send<room::res_checkin>(topic::room_res_checkin, res);
  }
}

}  // namespace alpha