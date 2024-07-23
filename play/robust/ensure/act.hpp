#pragma once

#include <play/robust/app/act.hpp>
#include <play/robust/base/stop_watch.hpp>
#include <string>

namespace play { namespace robust { namespace ensure {

class act : public app::act
{
public:
  act(app::actor& owner, act::ptr parent, const std::string& name)
      : app::act(owner),
        parent_{parent},
        name_{name}
  {
    build_path();
  }

  bool activate();

  void deactivate();

  const std::string& get_path() const
  {
    return path_;
  }

  const std::string& get_name() const
  {
    return name_;
  }

  ptr get_parent() const
  {
    return parent_;
  }

  base::stop_watch& get_stop_watch()
  {
    return stop_watch_;
  }

protected:
  virtual bool on_activate();

  virtual void on_deactivate();

private:
  void build_path();

private:
  base::stop_watch stop_watch_;
  std::string name_;
  act::ptr parent_;
  std::string path_;
};

}}}  // namespace play::robust::ensure