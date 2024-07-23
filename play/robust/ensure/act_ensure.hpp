#pragma once

#include <play/robust/app/act.hpp>
#include <play/robust/base/stop_watch.hpp>

namespace play { namespace robust { namespace ensure {

class act_ensure : public app::act
{
public:
  act_ensure(app::actor& owner)
      : app::act(owner)
  {
  }

  bool activate();

<<<<<<< HEAD:play/robust/ensure/ensure_act.hpp
  void deactivate();

  base::stop_watch& get_stop_watch() 
=======
  void deactivate() override;

  base::stop_watch& get_stop_watch()
>>>>>>> b61dcf63870f5463151f8d2f3f37818ae3cf2d71:play/robust/ensure/act_ensure.hpp
  {
    return stop_watch_;
  }

<<<<<<< HEAD:play/robust/ensure/ensure_act.hpp
protected: 
  virtual bool on_activate();

  virtual void on_deactivate();
=======
protected:
  virtual bool on_activate() = 0;

  virtual void on_deactivate() = 0;
>>>>>>> b61dcf63870f5463151f8d2f3f37818ae3cf2d71:play/robust/ensure/act_ensure.hpp

private:
  base::stop_watch stop_watch_;
};

}}}  // namespace play::robust::ensure