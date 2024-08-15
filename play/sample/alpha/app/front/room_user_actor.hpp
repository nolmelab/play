#pragma once

#include <alpha/share/fb/auth_generated.h>
#include <alpha/share/fb/room_generated.h>
#include <alpha/share/fb/topic_generated.h>
#include <alpha/app/actor.hpp>
#include <alpha/app/app.hpp>
#include <alpha/app/service.hpp>
#include <play/base/uuid.hpp>

namespace alpha {

class room_actor;

// room_runner 서비스 안에서 실행되는 방 액터
class room_user_actor : public actor
{
public:
  using ptr = std::shared_ptr<room_user_actor>;

public:
  room_user_actor(room_actor& room, app::pulse::session_ptr session, const std::string& name)
      : room_{room},
        session_{session},
        name_{name}
  {
  }

public:
  const std::string& get_name() const
  {
    return name_;
  }

private:
  bool on_start() final;

  void on_stop() final;

  void on_req_chat(app::pulse::session_ptr se, app::pulse::frame_ptr req);
  void on_closed(app::pulse::session_ptr se, app::pulse::frame_ptr req);

private:
  room_actor& room_;
  std::unique_ptr<app::pulse> pulse_;
  std::string name_;
  app::pulse::session_ptr session_;
};

}  // namespace alpha