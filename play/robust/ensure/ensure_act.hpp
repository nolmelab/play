#pragma once

#include <play/robust/app/act.hpp>
#include <play/robust/base/stop_watch.hpp>

namespace play { namespace robust { namespace ensure {

class ensure_act : public app::act
{
public:
  ensure_act(app::actor& owner)
      : app::act(owner)
  {
  }

  bool activate();

  void deactivate();

  base::stop_watch& get_stop_watch() 
  {
    return stop_watch_;
  }

protected: 
  virtual bool on_activate();

  virtual void on_deactivate();

private:
  base::stop_watch stop_watch_;
};

}}}  // namespace play::robust::ensure