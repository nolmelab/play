#include <alpha/share/fb/topic_generated.h>
#include <alpha/app/front/actor_user.hpp>
#include <alpha/app/front/lobby_runner.hpp>

namespace alpha {

bool lobby_runner::start()
{
  return true;
}

void lobby_runner::stop()
{
  // not much to do.
}

}  // namespace alpha