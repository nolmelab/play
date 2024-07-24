#include <play/robust/base/macros.hpp>
#include <play/robust/ensure/act.hpp>
#include <stack>

namespace play { namespace robust { namespace ensure {

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
  // 내가 타겟
  if (path == get_name())
  {
  }
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
  // read command and call functions
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

  path_.append("/");  // like directory, full path begins with /

  while (!path_stack.empty())
  {
    auto name = path_stack.top();
    path_.append(name);
    path_.append("/");
    path_stack.pop();
  }

  path_.append(get_name());
}

bool act::path::is_absolute_path(const std::string& path) {}

bool act::path::is_relative_path(const std::string& path) {}

std::string act::path::get_last_act_name(const std::string& path) {}

std::string act::path::is_chlid_of(const std::string& self_path, const std::string& path) {}

std::string act::path::get_child_path(const std::string& self_path, const std::string& path) {}

}}}  // namespace play::robust::ensure