#pragma once

#include <nlohmann/json.hpp>
#include <play/ensure/act.hpp>
#include <play/ensure/act_serial.hpp>
#include <play/net/runner/timer_service.hpp>

namespace ensure {

class bot;

// 순차적으로 act들을 관리하면서 실행
/**
 *  
 */
class flow : public act_serial
{
public:
  using act_ptr = std::shared_ptr<act>;

public:
  flow(play::actor& bt, const nlohmann::json& json);

private:
  void exit() final;
};

}  // namespace ensure