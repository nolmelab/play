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

  pulse_back_ = std::make_unique<app::pulse>();
  pulse_back_->as_client(&get_app().get_runner(), "127.0.0.1:8000")
      .sub(app::pulse::topic_estalished, PULSE_FN(on_established_back))
      .sub(app::pulse::topic_closed, PULSE_FN(on_closed_back))
      .sub(alpha::topic::auth_syn_login_b2f, PULSE_FN(on_auth_syn_login_b2f))
      .start();

  return true;
}

void lobby_runner::stop()
{
  pulse_front_.reset();
}

void lobby_runner::on_auth_req_login(app::pulse::session_ptr se, app::pulse::frame_ptr req)
{
  auto req_login = std::static_pointer_cast<alpha::auth::req_loginT>(req);

  auth::req_login_f2bT req_back;
  req_back.user_id = req_login->user_id;
  req_back.password = req_login->password;

  // 백앤드 서버의 세션을 알아야 한다. 여기에 미묘한 점들이 있다. 조건을 만족할 수 있는
  // 구조를 만들어야 한다.

  pulse_front_->call<auth::req_login_f2b>(se, alpha::topic::auth_req_logout_f2b,
                                          alpha::topic::auth_syn_login_b2f, req_back,
                                          [req_back]()
                                          {
                                            // process fail by close
                                          });
}

void lobby_runner::on_auth_syn_login_b2f(app::pulse::session_ptr se, app::pulse::frame_ptr req)
{
  //
}

void lobby_runner::on_established_back(app::pulse::session_ptr se, app::pulse::frame_ptr req)
{
  //
}

void lobby_runner::on_closed_back(app::pulse::session_ptr se, app::pulse::frame_ptr req)
{
  //
}

void lobby_runner::on_closed_front(app::pulse::session_ptr se, app::pulse::frame_ptr req)
{
  // find users by session
}

}  // namespace alpha