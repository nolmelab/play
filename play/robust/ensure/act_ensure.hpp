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

  // timeout 타이머를 등록. 주기적으로 확인.
  bool activate() override;

  void deactivate() override;

  base::stop_watch& get_stop_watch()
  {
    return stop_watch_;
  }

protected:
  virtual bool on_activate() = 0;

  virtual void on_deactivate() = 0;

private:
  base::stop_watch stop_watch_;
};

}}}  // namespace play::robust::ensure