#pragma once

#include <play/robust/app/act.hpp>

namespace play { namespace robust { namespace ensure {

class ensure_act : public app::act
{
public:
  ensure_act(app::actor& owner)
      : app::act(owner)
  {
  }

  bool activate() override;

  void deactivate() override;

  void destroy() override;
};

}}}  // namespace play::robust::ensure