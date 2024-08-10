#include <play/app/error.hpp>
#include <play/base/json_reader.hpp>
#include <play/base/macros.hpp>
#include <play/base/string_util.hpp>
#include <play/ensure/act_ensure.hpp>
#include <play/ensure/bot.hpp>
#include <stack>

namespace play {

bool act_ensure::activate()
{
  if (!active_)
  {
    active_ = on_activate();
    if (active_)
    {
      ++active_count_;
    }
    return active_;
  }
  return true;  // already active
}

void act_ensure::update()
{
  on_update();
}

void act_ensure::deactivate()
{
  if (active_)
  {
    on_deactivate();
    active_ = false;
  }
}

void act_ensure::jump(const std::string& path)
{
  if (has_parent())
  {
    get_parent()->jump(path);
  }
}

void act_ensure::next()
{
  if (has_parent())
  {
    get_parent()->next();
  }
}

void act_ensure::exit()
{
  if (has_parent())
  {
    get_parent()->exit();
  }
}

act_ensure::ptr act_ensure::find(const std::string& path)
{
  if (path.empty())
  {
    return {};
  }

  if (is_self(path))
  {
    return self();
  }

  if (path::is_absolute_path(path))
  {
    // path가 나의 자식인지 확인
    if (path::is_child_of(get_path().full_path_, path))
    {
      auto child_path = path::get_child_path(get_path().full_path_, path);
      auto child = find_child(child_path);
      if (child)
      {
        return child;
      }
      return {};
    }
    return find_up(path);
  }
  else
  {
    PLAY_CHECK(path::is_relative_path(path))
    auto first_act = path::get_first_act(path);
    if (first_act == get_path().act_name_)  // 내 이름으로 먼저 시작하는 경우
    {
      auto child_path = path::get_child_path(path);
      auto child = find_child(child_path);
      if (child)
      {
        return child;
      }
      return {};
    }
    // else - 자식 이름으로 시작하는 경우
    auto child = find_child(path);
    if (child)
    {
      return child;
    }
    return {};
  }
}

bool act_ensure::is_self(const std::string& path) const
{
  if (path::is_relative_path(path))
  {
    return path::get_last_act(path) == get_path().act_name_;
  }
  return path == get_path().full_path_;
}

bool act_ensure::on_activate()
{
  return true;
}

void act_ensure::on_update() {}

void act_ensure::on_deactivate() {}

void act_ensure::on_load_json() {}

void act_ensure::signal(std::string_view sig, std::string_view message)
{
  auto jslots = json_["slots"];
  if (jslots.is_object())
  {
    // command: { "success" : { "cmd" : "jump", "target" : "/battle/move"}}
    auto jsig = play::json_reader::read(jslots, std::string{sig}, nlohmann::json{});
    if (jsig.is_object())
    {
      auto cmd = play::json_reader::read(jsig, "cmd", std::string{});

      if (cmd == "jump")
      {
        auto target = play::json_reader::read(jsig, "target", std::string{});
        jump(target);  // XXX: target check? how?
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
        LOG()->error("unknown command: {} in act_ensure: {}", cmd, get_name());
      }
    }
    else
    {
      LOG()->error("signal: {} when slots are empty in act_ensure: {}", sig, get_name());
    }
  }
}

act_ensure::ptr act_ensure::find_up(const std::string& path)
{
  if (has_parent())
  {
    return get_parent()->find(path);
  }
  return {};
}

act_ensure::ptr act_ensure::find_child(const std::string& path)
{
  return {};
}

void act_ensure::build_path()
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

bool act_ensure::path::is_absolute_path(const std::string& path)
{
  return (path.length() > 0 && path[0] == '/');
}

bool act_ensure::path::is_relative_path(const std::string& path)
{
  return (path.length() > 0 && path[0] != '/');
}

std::string act_ensure::path::get_last_act(const std::string& path)
{
  PLAY_CHECK(!path.empty());
  auto parts = play::string_util::split(path, "/");
  return parts.back();
}

bool act_ensure::path::is_child_of(const std::string& self_path, const std::string& path)
{
  return (path.substr(0, self_path.length()) == self_path);
}

std::string act_ensure::path::get_child_path(const std::string& self_path, const std::string& path)
{
  if (act_ensure::path::is_child_of(self_path, path))
  {
    return path.substr(self_path.length() + 1);
  }
  return {};
}

std::string act_ensure::path::get_child_path(const std::string& path)
{
  return pop_head_act(path);
}

std::string act_ensure::path::get_first_act(const std::string& pa)
{
  if (pa.empty())
  {
    return {};
  }

  path p;
  p.setup(pa);
  return *p.parts_.begin();
}

std::string act_ensure::path::pop_head_act(const std::string& pa)
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

void act_ensure::path::setup(const std::string& full_path)
{
  full_path_ = full_path;
  parts_ = play::string_util::split(full_path_, "/");

  // 이와 같은 입력의 치명적인 실수는 예외를 쓸 수 밖에 없음
  if (parts_.empty())
  {
    throw play::error::create("invalid act_ensure path: {}", full_path);
  }

  act_name_ = parts_.back();
}

void act_ensure::load_json()
{
  on_load_json();
}

}  // namespace play