#include <alpha/ensure/act/act_client_lobby.hpp>
#include <alpha/ensure/act/act_connect_lobby.hpp>
#include <play/base/json_reader.hpp>
#include <play/ensure/bot.hpp>
#include <play/net/util/address.hpp>

namespace alpha {

bool act_connect_lobby::on_activate()
{
  get_bot().create_act<act_client_lobby>();

  auto cp = get_bot().get_act<act_client_lobby>();
  cp->get_handler().sub_session_strand(ACT_SESSION_CB(on_session_state), get_owner().get_id());

  auto addr = play::address::parse(address_);
  return cp->get_client().connect(addr.first, addr.second);
}

void act_connect_lobby::on_load_json()
{
  address_ = play::json_reader::read(get_json(), "address", std::string{"127.0.0.1:7000"});
}

void act_connect_lobby::on_session_state(session_ptr session, frame_handler::session_state state)
{
  if (state == frame_handler::session_state::established)
  {
    LOG()->info("session established. handle: {}", session->get_handle());
    succed("session establisehd");
  }
  else
  {
    LOG()->info("session closed. handle: {}", session->get_handle());
    // what to do?
  }
}

}  // namespace alpha