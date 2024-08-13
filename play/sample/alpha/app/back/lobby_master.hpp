#pragma once

#include <alpha/app/service.hpp>

namespace alpha {

// 인증, 중복 로그인 처리
class lobby_master : public service
{
public:
  lobby_master(app& app)
      : service(app)
  {
  }

  bool start() final;

  void stop() final;
};

}  // namespace alpha
