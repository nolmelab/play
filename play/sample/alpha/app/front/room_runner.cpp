#include <alpha/app/front/room_runner.hpp>

namespace alpha {

bool room_runner::start()
{
  // sub topics
  // timers
  return true;
}

void room_runner::stop()
{
  // not much to do.
}

void room_runner::on_room_req_checkin(session_ptr session, frame_ptr frame) {}

void room_runner::on_room_req_leave(session_ptr session, frame_ptr frame) {}

}  // namespace alpha