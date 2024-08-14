#pragma once

#include <alpha/app/service.hpp>

namespace alpha {

// 자체 strand에서 타이머, 메세지를 처리하는 로비
class lobby_runner : public service
{
public:
  lobby_runner(app& app)
      : service(app)
  {
  }

  bool start() final;

  void stop() final;

private:
  void on_established_back(app::pulse::session_ptr se, app::pulse::frame_ptr req);
  void on_closed_back(app::pulse::session_ptr se, app::pulse::frame_ptr req);
  void on_closed_front(app::pulse::session_ptr se, app::pulse::frame_ptr req);

  void on_auth_req_login(app::pulse::session_ptr se, app::pulse::frame_ptr req);
  void on_auth_syn_login_b2f(app::pulse::session_ptr se, app::pulse::frame_ptr req);
  void on_auth_syn_logout_b2f(app::pulse::session_ptr se, app::pulse::frame_ptr req);

private:
  std::unique_ptr<app::pulse> pulse_back_;
  std::unique_ptr<app::pulse> pulse_front_;
};

}  // namespace alpha
