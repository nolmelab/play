#pragma once

#include <play/app/service.hpp>

namespace alpha {

class alpha_app;

// 알파 서비스의 기반 클래스
class service_base : public play::service
{
public:
  service_base(alpha_app& app);

  alpha_app& get_app()
  {
    return app_;
  }

private:
  alpha_app& app_;
};

}  // namespace alpha
