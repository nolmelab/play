#pragma once

#include <play/ensure/act_ensure.hpp>

namespace play {

class act_composite : public act_ensure
{
public:
  act_composite(play::actor& owner, act_ensure::ptr parent, const nlohmann::json& json,
                const std::string& name)
      : act_ensure(owner, parent, json, name)
  {
  }

protected:
  bool load_acts();

  const std::vector<act_ensure::ptr>& get_acts() const
  {
    return acts_;
  }

private:
  std::vector<act_ensure::ptr> acts_;  // this is the sequential flow
};

}  // namespace play