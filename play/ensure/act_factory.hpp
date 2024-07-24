#pragma once

#include <functional>
#include <map>
#include <play/ensure/act.hpp>

namespace play { namespace ensure {

class act_factory
{
public:
  using creator =
      std::function<act::ptr(app::actor&, act::ptr, const nlohmann::json&, const std::string&)>;

public:
  static act_factory& get();

public:
  act::ptr create(const std::string& type, app::actor& owner, act::ptr parent,
                  const nlohmann::json& json, const std::string& name);

  void reg(std::string_view type, creator fn);

private:
  using creator_map = std::map<std::string, creator>;

private:
  creator_map creators_;
};

<<<<<<< HEAD:play/ensure/act_factory.hpp
}}  // namespace play::ensure
=======
}}}  // namespace play::robust::ensure

#define PLAY_REG_ACT(act_type)                                                                    \
  act::factory::get().reg(                                                                        \
      #act_type,                                                                                  \
      [](app::actor& owner, act::ptr parent, const nlohmann::json& json, const std::string& name) \
      {                                                                                           \
        return std::make_shared<act_type>(owner, parent, json, name);                             \
      })
>>>>>>> 91c59b068a24bbbadad9bbc480851488feb7e86b:play/robust/ensure/act_factory.hpp
