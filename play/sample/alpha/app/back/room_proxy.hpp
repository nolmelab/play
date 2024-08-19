#pragma once

#include <alpha/app/actor.hpp>
#include <alpha/app/app.hpp>

namespace alpha {

class room_runner_proxy;

// 리모트 tcp 주소로 이름을 사용하고 전송 용도와 방 개수 등 저장에만 사용
class room_proxy : public actor
{
public:
  using ptr = std::shared_ptr<room_runner_proxy>;

public:
  room_proxy(room_runner_proxy& runner_proxy, const std::string& name, app::pulse::session_ptr se)
      : runner_proxy_{runner_proxy},
        name_{name},
        session_{se}
  {
  }

  const std::string& get_name() const
  {
    return name_;
  }

  uintptr_t get_session_key() const
  {
    return reinterpret_cast<uintptr_t>(session_.get());
  }

  app::pulse* get_pulse()
  {
    return pulse_.get();
  }

private:
  bool on_start() final;

  void on_stop() final;

  void on_room_syn_leave_f2b(app::pulse::session_ptr se, app::pulse::frame_ptr fr);

  void on_room_res_reserve_f2b(app::pulse::session_ptr se, app::pulse::frame_ptr fr);

private:
  room_runner_proxy& runner_proxy_;
  std::string name_;
  app::pulse::session_ptr session_;
  std::unique_ptr<app::pulse> pulse_;
};

}  // namespace alpha
