#pragma once

#include <functional>
#include <map>
#include <play/ensure/act_ensure.hpp>

namespace play {

class act_factory
{
public:
  using creator = std::function<act_ensure::ptr(play::actor&, act_ensure::ptr,
                                                const nlohmann::json&, const std::string&)>;

public:
  static act_factory& get();

public:
  act_ensure::ptr create(const std::string& type, play::actor& owner, act_ensure::ptr parent,
                         const nlohmann::json& json, const std::string& name);

  void add_creator(std::string_view type, creator fn);

private:
  using creator_map = std::map<std::string, creator>;

private:
  creator_map creators_;
};

}  // namespace play

#define PLAY_REGISTER_ACT(act_type)                                                               \
  ::play::act_factory::get().add_creator(#act_type,                                               \
                                         [](::play::actor& owner, ::play::act_ensure::ptr parent, \
                                            const nlohmann::json& json, const std::string& name)  \
                                         {                                                        \
                                           return std::make_shared<act_type>(owner, parent, json, \
                                                                             name);               \
                                         })
