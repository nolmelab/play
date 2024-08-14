#pragma once

#include <alpha/app/service.hpp>

namespace alpha {

// 방 액터(인스턴스)들을 관리하는 서비스
class room_runner : public service
{
public:
  room_runner(app& app)
      : service(app)
  {
  }

  bool start() final;

  void stop() final;
};

}  // namespace alpha
