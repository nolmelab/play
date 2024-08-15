#include <alpha/app/front/lobby_runner.hpp>
#include <alpha/app/front/lobby_user_actor.hpp>

namespace alpha {

void lobby_user_actor::do_login(const alpha::auth::syn_login_b2fT& syn)
{
  if (syn.ec == error_code::success)
  {
    if (state_ == state::pending)
    {
      state_ = state::login;

      auth::res_loginT res;
      res.ec = error_code::success;
      pulse_->send<auth::res_login>(topic::auth_res_login, res);

      // 로그인 이후에 방 관련 프레임 구독
      pulse_->sub(topic::room_req_create, PULSE_FN(on_room_req_create))
          .sub(topic::room_req_page, PULSE_FN(on_room_req_page));
    }
    else
    {
      // duplicate login
      auth::syn_kickoutT res;
      res.ec = error_code::fail_kickout_by_duplicate_login;
      pulse_->send<auth::syn_kickout>(topic::auth_res_login, res);

      service_.post_del_user(get_id());
    }
  }
  else
  {
    auth::res_loginT res;
    res.ec = syn.ec;
    pulse_->send<auth::res_login>(topic::auth_res_login, res);

    service_.post_del_user(get_id());
  }
}

void lobby_user_actor::do_room_res_create_b2f(const alpha::room::res_createT& res)
{
  // TODO
}

void lobby_user_actor::do_room_res_reserve_b2f(const alpha::room::res_reserveT& res)
{
  // TODO
}

bool lobby_user_actor::on_start()
{
  PLAY_CHECK(session_);

  pulse_ = std::make_unique<app::pulse>();
  pulse_->as_child(service_.get_pulse_front())
      .with_strand(get_id())
      .with_session(session_)
      .sub(alpha::topic::auth_req_logout, PULSE_FN(on_auth_req_logout))
      .sub(app::pulse::topic_closed, PULSE_FN(on_session_closed))
      .start();

  auth::req_login_f2bT req_back;
  req_back.name = name_;
  req_back.password = password_;

  service_.get_pulse_back()->call<auth::req_login_f2b>(alpha::topic::auth_req_login_f2b,
                                                       alpha::topic::auth_syn_login_b2f, req_back,
                                                       [sp = shared_from_this(), this]()
                                                       {
                                                         on_login_fail_pending();
                                                       });

  return true;
}

void lobby_user_actor::on_stop()
{
  // close session?
}

void lobby_user_actor::on_login_fail_pending()
{
  auth::res_loginT res;
  res.ec = error_code::fail_backend_not_responding;
  pulse_->send<auth::res_login>(topic::auth_res_login, res);

  service_.post_del_user(get_id());
}

void lobby_user_actor::on_auth_req_logout(app::pulse::session_ptr se, app::pulse::frame_ptr req)
{
  auth::req_logout_f2bT req_back;
  req_back.name = name_;

  service_.get_pulse_back()->call<auth::req_logout_f2b>(topic::auth_req_logout_f2b, req_back,
                                                        []()
                                                        {
                                                          // empty
                                                        });
}

void lobby_user_actor::on_session_closed(app::pulse::session_ptr se, app::pulse::frame_ptr req)
{
  on_auth_req_logout(se, std::make_shared<auth::req_logoutT>());
}

void lobby_user_actor::on_room_req_create(app::pulse::session_ptr se, app::pulse::frame_ptr req)
{
  // a long trip
}

void lobby_user_actor::on_room_req_page(app::pulse::session_ptr se, app::pulse::frame_ptr req)
{
  // a short trip
}

}  // namespace alpha