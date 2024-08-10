#include <alpha/ensure/act/act_connect_lobby.hpp>
#include <play/base/json_reader.hpp>
#include <play/ensure/bot.hpp>
#include <play/net/util/address.hpp>

namespace alpha {

bool act_connect_lobby::on_activate()
{
  return true;
}

void act_connect_lobby::on_load_json()
{
  address_ = play::json_reader::read(get_json(), "address", std::string{"127.0.0.1:7000"});
}

}  // namespace alpha