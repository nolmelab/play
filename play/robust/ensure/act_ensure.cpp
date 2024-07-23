#include <play/robust/ensure/act_ensure.hpp>

namespace play { namespace robust { namespace ensure {

bool act_ensure::activate()
{
  // register timeout timer
  // access ensure application instance through bot

  return on_activate();
}

void act_ensure::deactivate()
{
  on_deactivate();
}

}}}  // namespace play::robust::ensure