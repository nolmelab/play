#pragma once

#include <alpha/share/fb/auth_generated.h>
#include <alpha/share/fb/room_generated.h>
#include <alpha/share/fb/topic_generated.h>
#include <alpha/app/actor.hpp>
#include <alpha/app/app.hpp>
#include <alpha/app/service.hpp>
#include <play/base/uuid.hpp>

namespace alpha {

class room_runner;

// room_runner 서비스 안에서 실행되는 방 액터
class room_actor : public actor
{
public:
  using ptr = std::shared_ptr<room_actor>;

  friend class room_runner;

public:
  room_actor(room_runner& service, const std::string& name, size_t capacity,
             const std::string& creator)
      : service_{service},
        name_{name},
        capacity_{capacity},
        creator_{creator}
  {
  }

public:
  const std::string& get_name() const
  {
    return name_;
  }

  const boost::uuids::uuid& get_uuid() const
  {
    return uuid_.get();
  }

  const std::string get_uuid_string() const
  {
    return uuid_.to_string();
  }

  app::pulse* get_pulse_front();

private:
  room_runner& get_service()
  {
    return service_;
  }

  bool on_start() final;

  void on_stop() final;

  void do_reserve(const room::req_reserveT& req);

  void do_chekin(const room::req_checkinT& req);

private:
  room_runner& service_;
  std::unique_ptr<app::pulse> pulse_;
  std::string name_;
  std::string creator_;
  size_t capacity_{0};
  play::uuid uuid_;
};

}  // namespace alpha