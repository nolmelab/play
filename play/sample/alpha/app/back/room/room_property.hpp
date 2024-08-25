#pragma once

#include <alpha/share/fb/room_generated.h>
#include <alpha/app/actor.hpp>
#include <alpha/app/app.hpp>

namespace alpha {

class room_property
{
public:
  room_property(const std::string& user_name, app::pulse::session_ptr se, room::room_infoT& info)
      : user_name_{user_name},
        session_{se},
        info_{info},
        pending_{true}
  {
  }

  const room::room_infoT& get_info() const
  {
    return info_;
  }

  const std::string& get_user_name() const
  {
    return user_name_;
  }

  uintptr_t get_session_key() const
  {
    return reinterpret_cast<uintptr_t>(session_.get());
  }

  void do_res_create();

  void do_reserve(const std::string& user_name);

  void do_checkin(const std::string& user_name);

  void do_leave(const std::string& user_name);

private:
  struct user
  {
    std::string name;
    bool checkin{false};
  };

private:
  room::room_infoT info_;
  bool pending_;
  std::string user_name_;
  app::pulse::session_ptr session_;
  std::vector<user> users_;
};

}  // namespace alpha
