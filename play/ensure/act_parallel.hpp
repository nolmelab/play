#pragma once

#include <play/base/macros.hpp>
#include <play/ensure/act_composite.hpp>

namespace play {

class act_parallel : public act_composite
{
public:
  act_parallel(play::actor& owner, act_ensure::ptr parent, const nlohmann::json& json,
               const std::string& name)
      : act_composite(owner, parent, json, name)
  {
  }

private:
  bool on_activate() final;

  void on_update() final;

  void on_deactivate() final;

  ptr find_child(const std::string& path) final;

  ptr find_act(const std::string& name) const;

  void jump(const std::string& path) final;

  void next() final;

  void jump_child(const std::string& path);
};

}  // namespace play