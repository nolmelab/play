#pragma once

#include <alpha/app/actor.hpp>
#include <play/base/uuid.hpp>

namespace alpha {

class user_master;

// 사용자 정보
class user_proxy : public actor
{
public:
  using ptr = std::shared_ptr<user_proxy>;

public:
  user_proxy(user_master& service, const std::string& name, app::pulse::session_ptr se,
             const std::string& password)
      : service_{service},
        name_{name},
        session_{se},
        password_{password}
  {
  }

  void do_logout();

  const std::string& get_name() const
  {
    return name_;
  }

  const boost::uuids::uuid& get_uuid() const
  {
    return uuid_.get();
  }

private:
  bool on_start() final;

  void on_stop() final;

private:
  user_master& service_;
  std::string name_;
  app::pulse::session_ptr session_;
  std::string password_;
  play::uuid uuid_;
};

}  // namespace alpha
