#include <alpha/app/front/room_actor.hpp>
#include <alpha/app/front/room_runner.hpp>

namespace alpha {

app::pulse* room_actor::get_pulse_front()
{
  return get_service().get_pulse_front();
}

void room_actor::do_reserve()
{
  //
}

void room_actor::do_join()
{
  //
}

bool room_actor::on_start()
{
  return true;
}

void room_actor::on_stop()
{
  // close session?
}

}  // namespace alpha