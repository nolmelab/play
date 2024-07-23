#include <play/robust/ensure/act.hpp>

namespace play { namespace robust { namespace ensure {

bool act::activate()
{
  // register timeout timer
  // access ensure application instance through bot

  return on_activate();
}

void act::deactivate()
{
  on_deactivate();
}

}}}  // namespace play::robust::ensure