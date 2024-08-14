#pragma once

#include <alpha/app/service.hpp>

namespace alpha {

class room_master : public service
{
public:
  room_master(app& app)
      : service(app)
  {
  }

  bool start() final;

  void stop() final;
};

}  // namespace alpha
