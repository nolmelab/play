#pragma once

#include <alpha/app/service_base.hpp>

namespace alpha {

// 자체 strand에서 타이머, 메세지를 처리하는 로비
class lobby_runner : public service_base
{
public:
  lobby_runner(alpha_app& app)
      : service_base(app)
  {
  }

  bool start() final;

  void stop() final;

private:
  void on_auth_req_login(session_ptr session, frame_ptr frame);

  void on_auth_req_logout(session_ptr session, frame_ptr frame);

  void on_session_state(session_ptr session, frame_handler::session_state state);

private:
  //
};

}  // namespace alpha
