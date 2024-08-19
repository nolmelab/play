#pragma once

#include <alpha/app/back/room/room_page_index.hpp>
#include <alpha/app/back/room_runner_proxy.hpp>
#include <alpha/app/service.hpp>
#include <play/app/actor_container.hpp>
#include <play/app/session_container.hpp>

namespace alpha {

class room_master : public service
{
public:
  room_master(app& app)
      : service(app)
  {
  }

  // 안전하게 post() 호출로 user를 지움
  void post_del_user(size_t id);

  app::pulse* get_pulse()
  {
    return pulse_.get();
  }

private:
  using room_runner_container =
      play::actor_container<room_runner_proxy, true, true, false>;  // id, name, session

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
  room_runner_container room_runners_;
  room_page_index room_page_index_;
};

}  // namespace alpha
