#include <alpha/app/front/room_actor.hpp>
#include <alpha/app/front/room_runner.hpp>

namespace alpha {

void room_actor::post_del_user(size_t id)
{
  pulse_->post(
      [this, id]()
      {
        this->users_.del(id);
      });
}

app::pulse* room_actor::get_pulse_front()
{
  return get_service().get_pulse_front();
}

app::pulse* room_actor::get_pulse_back()
{
  return get_service().get_pulse_back();
}

bool room_actor::on_start()
{
  pulse_ = std::make_unique<app::pulse>();
  pulse_->as_child(get_pulse_front()).with_strand(get_id()).start();

  return true;
}

void room_actor::on_stop()
{
  pulse_.reset();
}

void room_actor::do_reserve(const room::req_reserveT& req)
{
  if (capacity_ > (users_.size() + pending_users_.size()))
  {
    auto user = std::make_shared<room_user_actor>(*this, req.user_name);
    pending_users_.add(user);

    send_res_reserve_f2b(req.user_name, error_code::success);
  }
  else
  {
    send_res_reserve_f2b(req.user_name, error_code::fail_room_full);
  }
}

void room_actor::do_chekin(app::pulse::session_ptr se, const room::req_checkinT& req)
{
  auto uv = pending_users_.find(req.user_name);
  if (uv)
  {
    auto user = std::make_shared<room_user_actor>(*this, req.user_name, se);
    users_.add(user);
    user->start();

    pending_users_.del(req.user_name);

    room::res_checkinT res;
    res.ec = error_code::success;
    users_.for_each(
        [&res](room_user_actor::ptr up)
        {
          res.users.push_back(up->get_name());
        });

    pulse_->send<room::res_checkin>(se, topic::room_res_checkin, res);
  }
  else
  {
    room::res_checkinT res;
    res.ec = error_code::fail_room_user_not_found;
    pulse_->send<room::res_checkin>(se, topic::room_res_checkin, res);
  }
}

void room_actor::send_res_reserve_f2b(const std::string& user_name, error_code ec)
{
  room::res_reserveT res;
  res.ec = ec;
  res.user_name = user_name;
  res.room = std::make_unique<room::room_infoT>();
  res.room->uuid = uuid_.to_string();
  res.room->capacity = capacity_;
  res.room->current = users_.size() + pending_users_.size();

  get_service().get_pulse_back()->send<room::res_reserve>(topic::room_res_reserve_f2b, res);
}

}  // namespace alpha