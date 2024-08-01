#include <alpha/ensure/act/act_client_lobby.hpp>
#include <alpha/ensure/act/act_connect_lobby.hpp>
#include <play/base/json_reader.hpp>
#include <play/ensure/bot.hpp>

namespace alpha {

bool act_connect_lobby::on_activate()
{
  get_bot().create_act<act_client_lobby>();

  auto cp = get_bot().get_act<act_client_lobby>();

  // TODO: define ACT_SUB_TOPIC, ACT_SUB_SESSION
  // cp->get_client().connect();
}

void act_connect_lobby::on_load_json()
{
  address_ = play::json_reader::read(get_json(), "address", "127.0.0.1:7000");
}

}  // namespace alpha