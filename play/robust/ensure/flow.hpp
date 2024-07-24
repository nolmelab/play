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
  using act_ptr = std::shared_ptr<act>;

public:
  flow(app::actor& bt, const nlohmann::json& json);

private:
  bool on_activate() final;

  void on_deactivate() final;

  void jump(const std::string& path) final;

  void next() final;

  void exit() final;

  std::pair<size_t, act_ptr> find_act(const std::string& name) const;

  act_ptr get_current_act() const
  {
    return std::static_pointer_cast<act>(acts_[current_act_]);
  }

private:
  const nlohmann::json& json_;
  size_t current_act_;
  std::vector<act::ptr> acts_;  // this is the sequential flow
};

}}}  // namespace play::robust::ensure