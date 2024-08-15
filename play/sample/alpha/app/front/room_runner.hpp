#pragma once

#include <alpha/app/front/room_actor.hpp>
#include <alpha/app/service.hpp>
#include <play/app/actor_container.hpp>

namespace alpha {

// 방 액터(인스턴스)들을 관리하는 서비스
class room_runner : public service
{
public:
  room_runner(app& app)
      : service(app)
  {
  }

  void post_del_user(size_t id);

  app::pulse* get_pulse_front()
  {
    return pulse_front_.get();
  }

  app::pulse* get_pulse_back()
  {
    return pulse_back_.get();
  }

private:
  using room_container = play::actor_container<room_actor, true, false, true>;

private:
  bool on_start() final;

  void on_stop() final;

private:
  void on_established_back(app::pulse::session_ptr se, app::pulse::frame_ptr req);
  void on_closed_back(app::pulse::session_ptr se, app::pulse::frame_ptr req);

  void on_room_req_create_b2f(app::pulse::session_ptr se, app::pulse::frame_ptr req);
  void on_room_req_reserve_b2f(app::pulse::session_ptr se, app::pulse::frame_ptr fr);
  void on_room_req_checkin(app::pulse::session_ptr se, app::pulse::frame_ptr fr);

private:
  std::unique_ptr<app::pulse> pulse_back_;
  std::unique_ptr<app::pulse> pulse_front_;
  room_container rooms_;
};

}  // namespace alpha
