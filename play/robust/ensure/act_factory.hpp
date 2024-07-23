#pragma once

#include <functional>
#include <map>
#include <play/robust/ensure/act.hpp>

namespace play { namespace robust { namespace ensure {

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

}}}  // namespace play::robust::ensure