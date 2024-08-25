#pragma once

#include <alpha/app/actor.hpp>
#include <alpha/app/app.hpp>
#include <alpha/app/back/room/room_property.hpp>

namespace alpha {

class room_master;

class room_runner_property
{
public:
  room_runner_property(const std::string& remote_addr, app::pulse::session_ptr se)
      : remote_addr_{remote_addr},
        session_{se}
  {
  }

  const std::string& get_remote_addr() const
  {
    return remote_addr_;
  }

  uintptr_t get_session_key() const
  {
    return reinterpret_cast<uintptr_t>(session_.get());
  }

  size_t get_room_count() const
  {
    return rooms_.size();
  }

  void do_create_room(const room_property& room);

  void do_res_create(const std::string& uuid);

  void do_reserve(const std::string& uuid, const std::string& user_name);

  void do_checkin(const std::string& uuid, const std::string& user_name);

  void do_leave(const std::string& uuid, const std::string& user_name);

private:
  using room_map = std::map<std::string, room_property>;

private:
  std::string remote_addr_;
  app::pulse::session_ptr session_;
  room_map rooms_;
};

}  // namespace alpha
