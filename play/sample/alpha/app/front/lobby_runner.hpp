#pragma once

#include <alpha/app/service_base.hpp>

namespace alpha {

// 자체 strand에서 타이머, 메세지를 처리하는 로비
class lobby_runner : public service_base
{
public:
  lobby_runner(app& app)
      : service_base(app)
  {
  }

  bool start() final;

  void stop() final;

};

}  // namespace alpha
