<<<<<<< HEAD:play/ensure/act.cpp
#include <play/base/macros.hpp>
#include <play/ensure/act.hpp>
=======
#include <play/robust/base/json_reader.hpp>
#include <play/robust/base/macros.hpp>
#include <play/robust/base/string_util.hpp>
#include <play/robust/ensure/act.hpp>
>>>>>>> 91c59b068a24bbbadad9bbc480851488feb7e86b:play/robust/ensure/act.cpp
#include <stack>

namespace play { namespace ensure {

bool act::activate()
{
  PLAY_CHECK(!active_);

  on_load_json();
  active_ = on_activate();
  return active_;
}

void act::deactivate()
{
  on_deactivate();
  active_ = false;
}

void act::jump(const std::string& path)
{
  get_parent()->jump(path);
}

void act::next()
{
  LOG()->warn("act::next not implemented");
}

void act::exit()
{
  get_parent()->exit();
}

void act::signal(std::string_view sig, std::string_view message)
{
  auto jslots = json_["slots"];
  if (jslots.is_object())
  {
    // command: { "success" : { "cmd" : "jump", "target" : "/battle/move"}}
    auto jsig = base::json_reader::read(jslots, std::string{sig}, nlohmann::json{});
    if (jsig.is_object())
    {
      auto cmd = base::json_reader::read(jsig, "cmd", std::string{});

      if (cmd == "jump")
      {
        auto target = base::json_reader::read(jsig, "target", std::string{});
        jump(target);
      }
      else if (cmd == "next")
      {
        next();
      }
      else if (cmd == "exit")
      {
        exit();
      }
      else
      {
        LOG()->error("unknown command: {} in act: {}", cmd, get_name());
      }
    }
    else
    {
      LOG()->error("signal: {} when slots are empty in act: {}", sig, get_name());
    }
  }
}

void act::build_path()
{
  std::stack<std::string> path_stack;

  auto parent = get_parent();
  while (parent)
  {
    path_stack.push(parent->get_name());
    parent = parent->get_parent();
  }

  std::string full_path;

  full_path.append("/");  // like directory, full path begins with /

  while (!path_stack.empty())
  {
    auto name = path_stack.top();
    full_path.append(name);
    full_path.append("/");
    path_stack.pop();
  }

  full_path.append(get_name());
  path_.setup(full_path);
}

bool act::path::is_absolute_path(const std::string& path)
{
  return (path.length() > 0 && path[0] == '/');
}

bool act::path::is_relative_path(const std::string& path)
{
  return (path.length() > 0 && path[0] != '/');
}

std::string act::path::get_last_act(const std::string& path)
{
  PLAY_CHECK(!path.empty());
  auto parts = base::string_util::split(path, "/");
  return parts.back();
}

bool act::path::is_child_of(const std::string& self_path, const std::string& path)
{
  return (path.substr(0, self_path.length()) == self_path);
}

std::string act::path::get_child_path(const std::string& self_path, const std::string& path)
{
  if (act::path::is_child_of(self_path, path))
  {
    return path.substr(self_path.length());
  }
  return {};
}

std::string act::path::get_first_act(const std::string& pa)
{
  PLAY_CHECK(!pa.empty());
  path p;
  p.setup(pa);
  return *p.parts_.begin();
}

std::string act::path::pop_head_act(const std::string& pa)
{
  path p;
  p.setup(pa);

  if (p.parts_.size() <= 1)
    return {};

  std::string join;
  for (size_t i = 1; i < p.parts_.size() - 1; ++i)
  {
    join.append(p.parts_[i]);
    join.append("/");
  }
  join.append(p.parts_.back());
  return join;
}

void act::path::setup(const std::string& full_path)
{
  full_path_ = full_path;
  parts_ = base::string_util::split(full_path_, "/");
  act_name_ = parts_.back();
}

}}  // namespace play::ensure