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

private:
  bool on_start() final;

  void on_stop() final;
};

}  // namespace alpha
