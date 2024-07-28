#include <alpha/app/front/actor_user.hpp>
#include <alpha/app/front/lobby_runner.hpp>

namespace alpha {

bool lobby_runner::start()
{
  // get_handler().sub_topic();

  get_handler().sub_session_strand(
      [this](session_ptr s, frame_handler::session_state state)
      {
        this->on_session_state(s, state);
      },
      get_id());

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