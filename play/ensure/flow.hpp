#pragma once

#include <nlohmann/json.hpp>
#include <play/ensure/act.hpp>
#include <play/ensure/act_serial.hpp>
#include <play/net/runner/timer_service.hpp>

namespace play { namespace ensure {

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
  flow(app::actor& bt, const nlohmann::json& json);

  // ensure와 연결하여 주기적인 update 호출을 시작. activate() 호출
  bool start();

  // 주기적인 update 타이머 취소. deactivate() 호출
  void stop();

private:
  void exit() final;

  void update_acts();

private:
  net::timer::ref update_timer_;
};

}}  // namespace play::ensure