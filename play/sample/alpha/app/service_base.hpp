#pragma once

#include <alpha/app/alpha_app.hpp>
#include <play/app/service.hpp>

namespace alpha {

// 알파 서비스의 기반 클래스
class service_base : public play::service
{
public:
  using frame_ptr = std::shared_ptr<alpha_app::frame>;
  using session_ptr = std::shared_ptr<alpha_app::session>;
  using frame_handler = alpha_app::frame_handler;

public:
  service_base(alpha_app& app)
  : app_{app}
  {}

  alpha_app& get_app()
  {
    return app_;
  }

  alpha_app::frame_handler& get_handler()
  {
    return app_.get_handler();
  }

  play::runner& get_runner()
  {
    return app_.get_runner();
  }

private:
  alpha_app& app_;
};

}  // namespace alpha
