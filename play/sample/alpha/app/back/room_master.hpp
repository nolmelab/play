#pragma once

#include <alpha/app/service_base.hpp>

namespace alpha {

class room_master : public service_base
{
public:
  room_master(app& app)
      : service_base(app)
  {
  }

  bool start() final;

  void stop() final;
};

}  // namespace alpha
