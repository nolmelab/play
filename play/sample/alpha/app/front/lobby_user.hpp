#pragma once

#include <alpha/app/actor.hpp>
#include <alpha/app/service.hpp>

namespace alpha {

// lobby_runner 서비스 안에서 실행되는 사용자 액터
/**
 * 인증 후 이루어지는 이벤트 처리를 담당
 */
class lobby_user : public actor
{
public:
  lobby_user(service& service)
      : service_{service}
  {
  }

private:
  bool start() final;

  void stop() final;

private:
  service& get_service()
  {
    return service_;
  }

  template <typename Service>
  Service& get_service()
  {
    return static_cast<Service&>(get_service());
  }

private:
  service& service_;
};

}  // namespace alpha