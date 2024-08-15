#include <alpha/app/front/room_actor.hpp>
#include <alpha/app/front/room_runner.hpp>

namespace alpha {

app::pulse* room_actor::get_pulse_front()
{
  return get_service().get_pulse_front();
}

bool room_actor::on_start()
{
  return true;
}

void room_actor::on_stop()
{
  // close session?
}

void room_actor::do_reserve(const room::req_reserveT& req)
{
  // when capacity has enough users (this can fail since multiple users can reserve)
  // create room_user_actor
  // add
  // reply
}

void room_actor::do_chekin(const room::req_checkinT& req)
{
  // find the reserved user
  // change the state
}

}  // namespace alpha