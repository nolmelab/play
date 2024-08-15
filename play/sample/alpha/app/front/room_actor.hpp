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

public:
  room_actor(room_runner& service, const std::string& name, size_t capacity,
             const std::string& creator)
      : service_{service},
        name_{name},
        capacity_{capacity},
        creator_{creator}
  {
  }

  void do_reserve();

  void do_join();

public:
  const std::string& get_name() const
  {
    return name_;
  }

  const boost::uuids::uuid& get_uuid() const
  {
    return uuid_.get();
  }

  app::pulse* get_pulse_front();

private:
  room_runner& get_service()
  {
    return service_;
  }

  bool on_start() final;

  void on_stop() final;

private:
  room_runner& service_;
  std::unique_ptr<app::pulse> pulse_;
  std::string name_;
  std::string creator_;
  size_t capacity_{0};
  play::uuid uuid_;
};

}  // namespace alpha