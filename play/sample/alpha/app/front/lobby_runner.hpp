#pragma once

#include <alpha/app/service_base.hpp>

namespace alpha {

// 자체 strand에서 타이머, 메세지를 처리하는 로비
class lobby_runner : public service_base
{
public:
  lobby_runner(app& app)
      : service_base(app)
  {
  }

  bool start() final;

  void stop() final;

private:
  void on_auth_req_login(app::pulse::session_ptr se, app::pulse::frame_ptr req);
  void on_auth_syn_login_b2f(app::pulse::session_ptr se, app::pulse::frame_ptr req);

private:
  std::unique_ptr<app::pulse> pulse_;
};

}  // namespace alpha
