#pragma once

#include <alpha/app/app.hpp>
#include <play/app/service.hpp>

namespace alpha {

// 알파 서비스의 기반 클래스
class service_base : public play::service
{
public:
  service_base(app& app)
      : app_{app}
  {
  }

  app& get_app()
  {
    return app_;
  }

  play::runner& get_runner()
  {
    return app_.get_runner();
  }

private:
  app& app_;
};

}  // namespace alpha
