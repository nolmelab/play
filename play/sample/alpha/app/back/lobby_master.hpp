#pragma once

#include <alpha/app/service_base.hpp>

namespace alpha {

// 인증, 중복 로그인 처리
class lobby_master : public service_base
{
public:
  lobby_master(alpha_app& app)
      : service_base(app)
  {
  }

  bool start() final;

  void stop() final;
};

}  // namespace alpha
