#pragma once

#include <alpha/app/app.hpp>
#include <play/app/service.hpp>

namespace alpha {

// 알파 서비스의 기반 클래스
class service : public play::service
{
public:
  service(app& app)
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

#define PULSE_FN(func)                                          \
  [this](app::pulse::session_ptr se, app::pulse::frame_ptr req) \
  {                                                             \
    func(se, req);                                              \
  }