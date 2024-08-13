#include <alpha/share/fb/auth_generated.h>
#include <alpha/share/fb/topic_generated.h>
#include <alpha/app/front/actor_user.hpp>
#include <alpha/app/front/lobby_runner.hpp>

namespace alpha {

bool lobby_runner::start()
{
  pulse_front_ = std::make_unique<app::pulse>();
  pulse_front_->as_child(&get_app().get_pulse())
      .with_strand(get_id())
      .sub(alpha::topic::auth_req_login, PULSE_FN(on_auth_req_login))
      .sub(app::pulse::topic_closed, PULSE_FN(on_closed_front))
      .start();

  // established 처리를 위해 임시 등록
  pulse_back_ = std::make_unique<app::pulse>();
  pulse_back_->as_child(&get_app().get_pulse_back())
      .sub(app::pulse::topic_estalished, PULSE_FN(on_established_back))
      .start();

  LOG()->info("lobby runner started");

  return true;
}

void lobby_runner::stop()
{
  pulse_front_.reset();
  pulse_back_.reset();

  LOG()->info("lobby runner stopped");
}

void lobby_runner::on_established_back(app::pulse::session_ptr se, app::pulse::frame_ptr req)
{
  // 다시 펄스를 만들면서 세션을 연결. 연결이 끊어지고 다시 맺어지면 자동 복구
  pulse_back_.reset();
  pulse_back_ = std::make_unique<app::pulse>();
  pulse_back_->as_child(&get_app().get_pulse_back())
      .with_session(se)
      .sub(app::pulse::topic_estalished, PULSE_FN(on_established_back))
      .sub(app::pulse::topic_closed, PULSE_FN(on_closed_back))
      .sub(alpha::topic::auth_syn_login_b2f, PULSE_FN(on_auth_syn_login_b2f))
      .sub(alpha::topic::auth_syn_logout_b2f, PULSE_FN(on_auth_syn_logout_b2f))
      .start();

  // TODO: 서비스간 통지 추가
}

void lobby_runner::on_closed_back(app::pulse::session_ptr se, app::pulse::frame_ptr req)
{
  //
}

void lobby_runner::on_closed_front(app::pulse::session_ptr se, app::pulse::frame_ptr req)
{
  // find users by session
}

void lobby_runner::on_auth_req_login(app::pulse::session_ptr se, app::pulse::frame_ptr req)
{
  auto req_login = std::static_pointer_cast<alpha::auth::req_loginT>(req);

  auth::req_login_f2bT req_back;
  req_back.user_id = req_login->user_id;
  req_back.password = req_login->password;

  pulse_back_->call<auth::req_login_f2b>(alpha::topic::auth_req_logout_f2b,
                                         alpha::topic::auth_syn_login_b2f, req_back,
                                         [se, req_login]()
                                         {
                                           // 로그인 실패 응답
                                         });
}

void lobby_runner::on_auth_syn_login_b2f(app::pulse::session_ptr se, app::pulse::frame_ptr req)
{
  //
}

void lobby_runner::on_auth_syn_logout_b2f(app::pulse::session_ptr se, app::pulse::frame_ptr req)
{
  //
}


}  // namespace alpha