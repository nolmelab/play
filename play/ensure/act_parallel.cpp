#include <play/ensure/act_parallel.hpp>

namespace play {

bool act_parallel::on_activate()
{
  if (load_acts())
  {
    for (auto& ap : get_acts())
    {
      auto rc = ap->activate();
      if (!rc)
        return false;
    }
    return true;
  }

  return false;
}

void act_parallel::on_update()
{
  for (auto& ap : get_acts())
  {
    ap->update();
  }
}

void act_parallel::on_deactivate()
{
  for (auto& ap : get_acts())
  {
    ap->deactivate();
  }
}

void act_parallel::jump(const std::string& path)
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

void act_parallel::next()
{
  PLAY_CHECK(!"act_parallel does not support next");
  LOG()->error("act_parallel does not support next");
}

void act_parallel::jump_child(const std::string& pa)
{
  PLAY_CHECK(path::is_relative_path(pa));

  auto first_act = path::get_first_act(pa);
  auto ap = find_act(first_act);
  if (ap)
  {
    ap->jump(path::pop_head_act(pa));
  }
}

act_ensure::ptr act_parallel::find_child(const std::string& path)
{
  auto ap = find_act(path::get_first_act(path));
  if (ap)
  {
    return ap->find(path);
  }
  return ap;
}

act_ensure::ptr act_parallel::find_act(const std::string& name) const
{
  for (size_t i = 0; i < get_acts().size(); ++i)
  {
    auto a = get_acts()[i];
    if (a->get_name() == name)
    {
      return a;
    }
  }
  return {};
}

}  // namespace play