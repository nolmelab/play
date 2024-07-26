#pragma once

#include <functional>
#include <map>
#include <play/ensure/act.hpp>

namespace ensure {

class act_factory
{
public:
  using creator =
      std::function<act::ptr(play::actor&, act::ptr, const nlohmann::json&, const std::string&)>;

public:
  static act_factory& get();

public:
  act::ptr create(const std::string& type, play::actor& owner, act::ptr parent,
                  const nlohmann::json& json, const std::string& name);

  void reg(std::string_view type, creator fn);

private:
  using creator_map = std::map<std::string, creator>;

private:
  creator_map creators_;
};

}  // namespace ensure

#define PLAY_REGISTER_ACT(act_type)                                                                \
  ::ensure::act_factory::get().reg(#act_type,                                                      \
                                   [](::play::actor& owner, ensure::act::ptr parent,               \
                                      const nlohmann::json& json, const std::string& name)         \
                                   {                                                               \
                                     return std::make_shared<act_type>(owner, parent, json, name); \
                                   })
