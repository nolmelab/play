#pragma once

#include <alpha/app/service_base.hpp>

namespace alpha {

class lobby_runner : public service_base
{
public:
  lobby_runner(alpha_app& app)
      : service_base(app)
  {
  }

  bool start() final;

  void stop() final;
};

}  // namespace alpha
