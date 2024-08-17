#pragma once

#include <alpha/app/actor.hpp>
#include <alpha/app/app.hpp>

namespace alpha {

class room_master;

// 리모트 tcp 주소로 이름을 사용하고 전송 용도와 방 개수 등 저장에만 사용
class room_runner_proxy : public actor
{
public:
  using ptr = std::shared_ptr<room_runner_proxy>;

public:
  room_runner_proxy(room_master& master, const std::string& remote_addr, app::pulse::session_ptr se)
      : master_{master},
        remote_addr_{remote_addr},
        session_{se}
  {
  }

  const std::string& get_name() const
  {
    return remote_addr_;
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

private:
  room_master& master_;
  std::string remote_addr_;
  app::pulse::session_ptr session_;
  std::unique_ptr<app::pulse> pulse_;
};

}  // namespace alpha
