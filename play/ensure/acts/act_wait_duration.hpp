#pragma once

#include <play/ensure/act_ensure.hpp>

namespace play {

// duration으로 지정한 시간을 대기
class act_wait_duration : public act_ensure
{
public:
  act_wait_duration(play::actor& owner, act_ensure::ptr parent, const nlohmann::json& json,
                    const std::string& name);

private:
  void on_update() final;

  void on_load_json() final;

private:
  int duration_{0};
};

}  // namespace play