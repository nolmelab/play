#pragma once

#include <play/ensure/act.hpp>

namespace play { namespace ensure {

class act_composite : public act
{
public:
  act_composite(app::actor& owner, act::ptr parent, const nlohmann::json& json,
                const std::string& name)
      : act(owner, parent, json, name)
  {
  }

protected:
  bool load_acts();

  const std::vector<act::ptr>& get_acts() const
  {
    return acts_;
  }

private:
  std::vector<act::ptr> acts_;  // this is the sequential flow
};

}}  // namespace play::ensure