#pragma once

#include <nlohmann/json.hpp>
#include <play/robust/ensure/act_serial.hpp>

namespace play { namespace robust { namespace ensure {

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

private:
  void exit() final;
};

}}}  // namespace play::robust::ensure