#pragma once

#include <alpha/share/fb/auth_generated.h>
#include <alpha/share/fb/topic_generated.h>
#include <alpha/app/actor.hpp>
#include <alpha/app/app.hpp>
#include <alpha/app/service.hpp>

namespace alpha {

class lobby_runner;

// lobby_runner 서비스 안에서 실행되는 사용자 액터
/**
 * 인증 후 이루어지는 이벤트 처리를 담당
 */
class lobby_user : public actor
{
public:
  using ptr = std::shared_ptr<lobby_user>;

  enum class state
  {
    pending,
    login
  };

public:
  lobby_user(lobby_runner& service, const std::string& name, app::pulse::session_ptr se,
             const std::string& password)
      : service_{service},
        name_{name},
        session_{se},
        password_{password}
  {
  }

public:
  void do_login(const alpha::auth::syn_login_b2fT& syn);

  const std::string& get_name() const
  {
    return name_;
  }

  uintptr_t get_session_key() const
  {
    return reinterpret_cast<uintptr_t>(session_.get());
  }

private:
  lobby_runner& get_service()
  {
    return service_;
  }

  bool on_start() final;

  void on_stop() final;

  void on_login_fail_pending();

  void on_auth_req_logout(app::pulse::session_ptr se, app::pulse::frame_ptr req);

  void on_session_closed(app::pulse::session_ptr se, app::pulse::frame_ptr req);

private:
  lobby_runner& service_;
  std::string name_;
  app::pulse::session_ptr session_;
  state state_{state::pending};
  std::unique_ptr<app::pulse> pulse_;
  std::string password_;
};

}  // namespace alpha