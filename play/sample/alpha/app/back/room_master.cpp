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
  auto req = std::static_pointer_cast<room::syn_runner_upT>(fr);
  auto runner = room_runner_property(se->get_remote_addr(), se);
  auto session_key = reinterpret_cast<uintptr_t>(se.get());
  room_runners_.insert(std::pair{session_key, runner});
}

void room_master::on_room_req_create_f2b(app::pulse::session_ptr se, app::pulse::frame_ptr fr)
{
  // 방 개수가 가장 적은 room_runner를 찾아서 생성 요청
  auto req = std::static_pointer_cast<room::req_createT>(fr);

  if (room_runners_.empty())
  {
    room::res_createT res;
    res.ec = error_code::fail_room_runner_not_found;
    res.user_name = req->user_name;
    res.room = std::move(req->room);
    pulse_->send<room::res_create>(topic::room_res_create_b2f, res);
    return;
  }

  // TODO: 더 효율적으로 처리할 방법은?
  std::vector<room_runner_property*> runners;
  for (auto& kv : room_runners_)
  {
    runners.push_back(&kv.second);
  }

  std::sort(runners.begin(), runners.end(),
            [](const room_runner_property* r1, const room_runner_property* r2)
            {
              return r1->get_room_count() > r2->get_room_count();
            });

  PLAY_CHECK(!runners.empty());
}

void room_master::on_room_res_create_f2b(app::pulse::session_ptr se, app::pulse::frame_ptr fr)
{
  // room_runner_proxy에서도 구독하여 자체적으로 추가 / 삭제 처리
  // 여기서는 room 페이지 관련 처리
  // 사용자 전송은 room_runner_proxy에서 처리
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
  // TODO: room index에 반영
}

}  // namespace alpha