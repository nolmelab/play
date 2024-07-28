#pragma once

#include <alpha/app/actor_base.hpp>
#include <alpha/app/service_base.hpp>

namespace alpha {

// lobby_runner 서비스 안에서 실행되는 사용자 액터
/**
 * 인증 후 이루어지는 이벤트 처리를 담당
 */
class actor_user : public actor_base
{
public:
  actor_user(service_base& service)
      : service_{service}
  {
  }

private:
  void on_room_req_create(session_ptr session, frame_ptr frame);

  void on_room_req_page(session_ptr session, frame_ptr frame);

  void on_room_req_join(session_ptr session, frame_ptr frame);

  service_base& get_service()
  {
    return service_;
  }

  template <typename Service>
  Service& get_service()
  {
    return static_cast<Service>(get_service());
  }

private:
  service_base& service_;
};

}  // namespace alpha