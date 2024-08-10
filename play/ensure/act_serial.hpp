#pragma once

#include <play/base/macros.hpp>
#include <play/ensure/act_composite.hpp>

namespace play {

class act_serial : public act_composite
{
public:
  act_serial(play::actor& owner, act_ensure::ptr parent, const nlohmann::json& json,
             const std::string& name)
      : act_composite(owner, parent, json, name)
  {
  }

protected:
  act_ensure::ptr get_current_act() const
  {
    PLAY_CHECK(current_act_index_ >= 0 && current_act_index_ < get_acts().size())
    return get_acts()[current_act_index_];
  }

  ptr find_child(const std::string& path) final;

  std::pair<size_t, act_ensure::ptr> find_act(const std::string& name) const;

private:
  bool on_activate() final;

  void on_update() final;

  void on_deactivate() final;

  void jump(const std::string& path) final;

  void next() final;

  // 자식에 대한 점프는 최상위 자식들에 대해서만 실행
  void jump_child(const std::string& path);

private:
  size_t current_act_index_{0};
};

}  // namespace play