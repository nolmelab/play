#include <alpha/share/fb/auth_generated.h>
#include <alpha/share/fb/topic_generated.h>
#include <alpha/app/back/user_master.hpp>

namespace alpha {

void user_master::post_del_user(size_t id)
{
  pulse_->post(
      [this, id]()
      {
        this->users_.del(id);
      });
}

bool user_master::on_start()
{
  pulse_ = std::make_unique<app::pulse>();
  pulse_->as_child(&get_app().get_pulse())
      .with_strand(get_id())
      .sub(app::pulse::topic_estalished, PULSE_FN(on_front_established))
      .sub(app::pulse::topic_closed, PULSE_FN(on_front_closed))
      .sub(alpha::topic::auth_req_login_f2b, PULSE_FN(on_auth_req_login_f2b))
      .sub(alpha::topic::auth_req_logout_f2b, PULSE_FN(on_auth_req_logout_f2b))
      .start();

  fronts_.bind(pulse_.get());

  return true;
}

void user_master::on_stop()
{
  pulse_.reset();
}

void user_master::on_auth_req_login_f2b(app::pulse::session_ptr se, app::pulse::frame_ptr fr)
{
  auto req = std::static_pointer_cast<auth::req_login_f2bT>(fr);
  auto user = std::make_shared<user_proxy>(*this, req->name, se, req->password);
  users_.add(user);
  user->start();

  // 개별 사용자에 제한되지 않는 처리가 필요하므로 user_master에서 처리한다.
  auth::syn_login_b2fT syn;
  syn.ec = error_code::success;
  syn.name = req->name;
  fronts_.send<auth::syn_login_b2f>(topic::auth_syn_login_b2f, syn);
}

void user_master::on_auth_req_logout_f2b(app::pulse::session_ptr se, app::pulse::frame_ptr fr)
{
  auto req = std::static_pointer_cast<auth::req_logout_f2bT>(fr);
  auto uv = users_.find(req->name);
  if (uv.has_value())
  {
    auto user = uv.value();
    user->do_logout();
    post_del_user(user->get_id());

    auth::syn_logout_b2fT syn;
    syn.name = user->get_name();
    fronts_.send<auth::syn_logout_b2f>(topic::auth_syn_login_b2f, syn);
  }
}

void user_master::on_front_established(app::pulse::session_ptr se, app::pulse::frame_ptr fr)
{
  fronts_.add(se);
}

void user_master::on_front_closed(app::pulse::session_ptr se, app::pulse::frame_ptr fr)
{
  fronts_.del(se);
}

}  // namespace alpha