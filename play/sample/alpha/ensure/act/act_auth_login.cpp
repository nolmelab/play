#include <alpha/share/fb/auth_generated.h>
#include <alpha/share/fb/topic_generated.h>
#include <alpha/ensure/act/act_auth_login.hpp>
#include <alpha/ensure/act/act_client_lobby.hpp>
#include <play/base/json_reader.hpp>
#include <play/ensure/bot.hpp>

namespace alpha {

bool act_auth_login::on_activate()
{
  auto cp = get_bot().get_act<act_client_lobby>();
  sub_id_ = cp->get_handler().sub_topic_strand((uint16_t)alpha::topic::auth_res_login,
                                               ACT_TOPIC_CB(on_res_login), get_owner().get_id());

  return req_login();
}

void act_auth_login::on_deactivate()
{
  auto cp = get_bot().get_act<act_client_lobby>();
  cp->get_handler().unsub((uint16_t)alpha::topic::auth_res_login, sub_id_);
}

bool act_auth_login::req_login()
{
  auto cp = get_bot().get_act<act_client_lobby>();

  alpha::auth::req_loginT req;
  req.user_id = "name";
  req.password = "pass";

  return cp->get_handler().send<alpha::auth::req_login>(
      cp->get_client().get_session(), (uint16_t)alpha::topic::auth_req_login, req, false);
}

void act_auth_login::on_res_login(session_ptr se, frame_ptr fr)
{
  // auto res = std::static_pointer_cast<alpha::auth::
}

}  // namespace alpha