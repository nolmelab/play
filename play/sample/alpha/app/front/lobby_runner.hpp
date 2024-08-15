#pragma once

#include <alpha/app/front/lobby_user_actor.hpp>
#include <alpha/app/service.hpp>
#include <play/app/actor_container.hpp>

namespace alpha {

// 자체 strand에서 타이머, 메세지를 처리하는 로비
class lobby_runner : public service
{
public:
  lobby_runner(app& app)
      : service(app)
  {
  }

  // 안전하게 post() 호출로 user를 지움
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
  using user_container = play::actor_container<lobby_user_actor, true, true, false>;

private:
  bool on_start() final;

  void on_stop() final;

private:
  void on_established_back(app::pulse::session_ptr se, app::pulse::frame_ptr req);
  void on_closed_back(app::pulse::session_ptr se, app::pulse::frame_ptr req);

  void on_auth_req_login(app::pulse::session_ptr se, app::pulse::frame_ptr req);
  void on_auth_syn_login_b2f(app::pulse::session_ptr se, app::pulse::frame_ptr req);
  void on_auth_syn_logout_b2f(app::pulse::session_ptr se, app::pulse::frame_ptr req);

  void on_room_res_create_b2f(app::pulse::session_ptr se, app::pulse::frame_ptr req);
  void on_room_res_reserve_b2f(app::pulse::session_ptr se, app::pulse::frame_ptr req);

private:
  std::unique_ptr<app::pulse> pulse_back_;
  std::unique_ptr<app::pulse> pulse_front_;
  user_container users_;
};

}  // namespace alpha
