#include <play/ensure/act_serial.hpp>

namespace play { namespace ensure {

bool act_serial::on_activate()
{
  if (load_acts())
  {
    PLAY_CHECK(get_current_act() == get_acts()[0]);
    return get_current_act()->activate();
  }

  return false;
}

void act_serial::on_deactivate()
{
  get_current_act()->deactivate();
}

void act_serial::jump(const std::string& path)
{
  if (path::is_relative_path(path))
  {
    jump_child(path);
  }
  else
  {
    if (path::is_child_of(get_path().full_path_, path))
    {
      jump_child(path::get_child_path(get_path().full_path_, path));
    }
    else
    {
      if (has_parent())
      {
        get_parent()->jump(path);
      }
      else
      {
        LOG()->warn("invalid jump path: {}. no act is found to process the requested jump", path);
      }
    }
  }
}

void act_serial::next()
{
  get_current_act()->deactivate();

  current_act_index_++;
  if (current_act_index_ >= get_acts().size())
  {
    current_act_index_--;  // recover to the previous one
    LOG()->error("flow reached the end");
  }
  else
  {
    get_current_act()->activate();
  }
}

void act_serial::jump_child(const std::string& path)
{
  auto first_act = path::get_first_act(path);
  auto ia = find_act(first_act);
  if (ia.second)
  {
    get_current_act()->deactivate();
    current_act_index_ = ia.first;
    get_current_act()->activate();

    get_current_act()->jump(path::pop_head_act(path));
  }
}

std::pair<size_t, act::ptr> act_serial::find_act(const std::string& name) const
{
  for (size_t i = 0; i < get_acts().size(); ++i)
  {
    auto a = get_acts()[i];
    if (a->get_name() == name)
    {
      return {i, a};
    }
  }
  return {0, {}};
}

}}  // namespace play::ensure