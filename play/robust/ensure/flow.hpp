#pragma once

#include <nlohmann/json.hpp>
#include <play/robust/ensure/act.hpp>

namespace play { namespace robust { namespace ensure {

class bot;

// 순차적으로 act들을 관리하면서 실행
/**
 *  
 */
class flow : public act
{
public:
  flow(app::actor& bt, const nlohmann::json& json);

private:
  bool on_activate();

  void on_deactivate();

private:
  const nlohmann::json& json_;
  size_t current_act_;
  std::vector<act::ptr> acts_;  // this is the sequential flow
};

}}}  // namespace play::robust::ensure