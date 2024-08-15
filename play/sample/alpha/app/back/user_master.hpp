#pragma once

#include <alpha/app/app.hpp>
#include <alpha/app/back/user_proxy.hpp>
#include <alpha/app/service.hpp>
#include <play/app/actor_container.hpp>
#include <play/app/session_container.hpp>

namespace alpha {

// 인증, 중복 로그인 처리
class user_master : public service
{
public:
  user_master(app& app)
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
  using user_container = play::actor_container<user_proxy, true, false, true>;  // id, name, uuid
  using session_container = play::session_container<app::pulse>;

private:
  bool on_start() final;

  void on_stop() final;

  void on_auth_req_login_f2b(app::pulse::session_ptr se, app::pulse::frame_ptr req);
  void on_auth_req_logout_f2b(app::pulse::session_ptr se, app::pulse::frame_ptr req);
  void on_front_established(app::pulse::session_ptr se, app::pulse::frame_ptr req);
  void on_front_closed(app::pulse::session_ptr se, app::pulse::frame_ptr req);

private:
  std::unique_ptr<app::pulse> pulse_;
  session_container fronts_;
  user_container users_;
};

}  // namespace alpha
