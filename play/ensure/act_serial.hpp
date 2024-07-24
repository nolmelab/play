#pragma once

#include <play/base/macros.hpp>
#include <play/ensure/act_composite.hpp>

namespace play { namespace ensure {

class act_serial : public act_composite
{
public:
public:
  act_serial(app::actor& owner, act::ptr parent, const nlohmann::json& json,
             const std::string& name)
      : act_composite(owner, parent, json, name)
  {
  }

protected:
  act::ptr get_current_act() const
  {
    PLAY_CHECK(current_act_index_ >= 0 && current_act_index_ < get_acts().size())
    return get_acts()[current_act_index_];
  }

  std::pair<size_t, act::ptr> find_act(const std::string& name) const;

private:
  bool on_activate() final;

  void on_deactivate() final;

  void jump(const std::string& path) final;

  void next() final;

  void jump_child(const std::string& path);

private:
  size_t current_act_index_{0};
};

}}  // namespace play::ensure