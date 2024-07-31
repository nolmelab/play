#include <alpha/share/fb/topic_generated.h>
#include <alpha/app/front/actor_user.hpp>
#include <alpha/app/front/lobby_runner.hpp>

namespace alpha {

bool lobby_runner::start()
{
  SERVICE_SUB_TOPIC(alpha::topic::auth_req_login, on_auth_req_login);
  SERVICE_SUB_TOPIC(alpha::topic::auth_req_logout, on_auth_req_logout);
  SERVICE_SUB_SESSION(on_session_state);

  return true;
}

void lobby_runner::stop()
{
  // not much to do.
}

void lobby_runner::on_auth_req_login(session_ptr session, frame_ptr frame)
{
  auto user = std::make_shared<actor_user>(*this);
  // put it into pending_users_

  // auto& rcs = get_app().get_service<rcs_service>();
  // rcs.call<auth_req_login_f2b>(shared_from_this(), req);
}

void lobby_runner::on_auth_req_logout(session_ptr session, frame_ptr frame)
{
  //
}

void lobby_runner::on_session_state(session_ptr session, frame_handler::session_state state)
{
  if (state == frame_handler::session_state::closed)
  {
    //
  }
}

}  // namespace alpha