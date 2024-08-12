#include <alpha/share/fb/auth_generated.h>
#include <alpha/share/fb/topic_generated.h>
#include <alpha/app/front/actor_user.hpp>
#include <alpha/app/front/lobby_runner.hpp>

namespace alpha {

bool lobby_runner::start()
{
  pulse_ = std::make_unique<app::pulse>();
  pulse_->as_child(&get_app().get_pulse())
      .with_strand(get_id())
      .subscribe(alpha::topic::auth_req_login, PULSE_FUNC(on_auth_req_login))
      .subscribe(alpha::topic::auth_syn_login_b2f, PULSE_FUNC(on_auth_syn_login_b2f))
      .start();
  return true;
}

void lobby_runner::stop()
{
  pulse_.reset();
}

void lobby_runner::on_auth_req_login(app::pulse::session_ptr se, app::pulse::frame_ptr req)
{
  auto req_login = std::static_pointer_cast<alpha::auth::req_loginT>(req);

  auth::req_login_f2bT req_back;
  req_back.user_id = req_login->user_id;
  req_back.password = req_login->password;

  // 백앤드 서버의 세션을 알아야 한다. 여기에 미묘한 점들이 있다. 조건을 만족할 수 있는
  // 구조를 만들어야 한다.

  pulse_->call<auth::req_login_f2b>(se, alpha::topic::auth_req_logout_f2b,
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

}  // namespace alpha