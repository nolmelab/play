#pragma once

#include <play/robust/base/macros.hpp>
#include <play/robust/ensure/act_composite.hpp>

namespace play { namespace robust { namespace ensure {

class act_parallel : public act_composite
{
public:
public:
  act_parallel(app::actor& owner, act::ptr parent, const nlohmann::json& json,
               const std::string& name)
      : act_composite(owner, parent, json, name)
  {
  }

protected:
  act::ptr find_act(const std::string& name) const;

private:
  void jump(const std::string& path) override;

  void jump_child(const std::string& path);
};

}}}  // namespace play::robust::ensure