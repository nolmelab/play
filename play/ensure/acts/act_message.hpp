#pragma once

#include <play/ensure/act.hpp>

namespace play { namespace ensure {

class act_message : public act
{
public:
  act_message(app::actor& owner, act::ptr parent, const nlohmann::json& json,
              const std::string& name);

private:
  bool on_activate() final;

  void on_update() final;
};

}}  // namespace play::ensure