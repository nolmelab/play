#pragma once

#include <alpha/app/back/room/room_page_index.hpp>
#include <alpha/app/back/room/room_runner_property.hpp>
#include <alpha/app/service.hpp>
#include <play/app/actor_container.hpp>
#include <play/app/index.hpp>
#include <play/app/session_container.hpp>

namespace alpha {

// 방 관리자. 방 생성과 소멸을 제어.
/**
 * room 페이지 조회와 room runner 별 방 관리를 한다. 
 */
class room_master : public service
{
public:
  room_master(app& app)
      : service(app)
  {
  }

private:
  using room_runner_map = std::map<uintptr_t, room_runner_property>;

private:
  bool on_start() final;

  void on_stop() final;

  void on_room_syn_runner_up(app::pulse::session_ptr se, app::pulse::frame_ptr fr);

  void on_room_req_create_f2b(app::pulse::session_ptr se, app::pulse::frame_ptr fr);

  void on_room_res_create_f2b(app::pulse::session_ptr se, app::pulse::frame_ptr fr);

  void on_room_req_page_f2b(app::pulse::session_ptr se, app::pulse::frame_ptr fr);

  void on_room_req_reserve_f2b(app::pulse::session_ptr se, app::pulse::frame_ptr fr);

  void on_room_res_reserve_f2b(app::pulse::session_ptr se, app::pulse::frame_ptr fr);

  void on_room_syn_leave_f2b(app::pulse::session_ptr se, app::pulse::frame_ptr fr);

  void on_front_established(app::pulse::session_ptr se, app::pulse::frame_ptr fr);

  void on_front_closed(app::pulse::session_ptr se, app::pulse::frame_ptr fr);

private:
  std::unique_ptr<app::pulse> pulse_;
  room_page_index room_page_index_;
  room_runner_map room_runners_;
};

}  // namespace alpha
