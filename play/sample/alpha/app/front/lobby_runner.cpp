#include <alpha/share/fb/auth_generated.h>
#include <alpha/share/fb/topic_generated.h>
#include <alpha/app/front/lobby_runner.hpp>
#include <alpha/app/front/lobby_user_actor.hpp>

namespace alpha {

void lobby_runner::post_del_user(size_t id)
{
  pulse_front_->post(
      [this, id]()
      {
        this->users_.del(id);
      });
}

bool lobby_runner::on_start()
{
  pulse_front_ = std::make_unique<app::pulse>();
  pulse_front_->as_child(&get_app().get_pulse())
      .with_strand(get_id())
      .sub(alpha::topic::auth_req_login, PULSE_FN(on_auth_req_login))
      .sub(alpha::topic::room_res_create_b2f, PULSE_FN(on_room_res_create_b2f))
      .start();

  // established 처리를 위해 임시 등록
  pulse_back_ = std::make_unique<app::pulse>();
  pulse_back_->as_child(&get_app().get_pulse_back())
      .with_strand(get_id())
      .mark_final()
      .sub(app::pulse::topic_estalished, PULSE_FN(on_established_back))
      .start();

  LOG()->info("lobby runner started");

  return true;
}

void lobby_runner::on_stop()
{
  pulse_front_.reset();
  pulse_back_.reset();

  LOG()->info("lobby runner stopped");
}

void lobby_runner::on_established_back(app::pulse::session_ptr se, app::pulse::frame_ptr req)
{
  // 다시 펄스를 만들면서 세션을 연결. 연결이 끊어지고 다시 맺어지면 자동 복구
  pulse_back_.reset();
  // XXX: 안전한가??
  pulse_back_ = std::make_unique<app::pulse>();
  pulse_back_->as_child(&get_app().get_pulse_back())
      .with_strand(get_id())
      .with_session(se)
      .sub(app::pulse::topic_estalished, PULSE_FN(on_established_back))
      .sub(alpha::topic::auth_syn_login_b2f, PULSE_FN(on_auth_syn_login_b2f))
      .sub(alpha::topic::auth_syn_logout_b2f, PULSE_FN(on_auth_syn_logout_b2f))
      .start();
}

void lobby_runner::on_auth_req_login(app::pulse::session_ptr se, app::pulse::frame_ptr fr)
{
  auto req = std::static_pointer_cast<alpha::auth::req_loginT>(fr);
  auto user = std::make_shared<lobby_user_actor>(*this, req->name, se, req->password);
  users_.add(user);
  user->start();
}

void lobby_runner::on_auth_syn_login_b2f(app::pulse::session_ptr se, app::pulse::frame_ptr fr)
{
  auto res = std::static_pointer_cast<alpha::auth::syn_login_b2fT>(fr);
  auto uv = users_.find(res->name);
  if (uv)
  {
    uv.value()->do_login(*res);
  }
}

void lobby_runner::on_auth_syn_logout_b2f(app::pulse::session_ptr se, app::pulse::frame_ptr fr)
{
  // 샘플 프로토콜에서는 특별히 할 일이 없음.
}

void lobby_runner::on_room_res_create_b2f(app::pulse::session_ptr se, app::pulse::frame_ptr fr)
{
  auto res = std::static_pointer_cast<room::res_createT>(fr);
  auto uv = users_.find(res->user_name);
  if (uv)
  {
    auto user = uv.value();
    user->do_room_res_create_b2f(*res);
  }
}

void lobby_runner::on_room_res_reserve_b2f(app::pulse::session_ptr se, app::pulse::frame_ptr fr)
{
  auto res = std::static_pointer_cast<room::res_reserveT>(fr);
  auto uv = users_.find(res->user_name);
  if (uv)
  {
    auto user = uv.value();
    user->do_room_res_reserve_b2f(*res);
  }
}

}  // namespace alpha