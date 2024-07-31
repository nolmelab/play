#include <alpha/ensure/act/act_connect_lobby.hpp>
#include <play/base/json_reader.hpp>

namespace alpha {

bool act_connect_lobby::on_activate()
{
  handler_ = std::make_unique<client::frame_handler>(get_bot().get_app().get_runner());
  auto cp = std::make_shared<client>(*handler_);

  // cp->connect(address_);
}

void act_connect_lobby::on_load_json()
{
  address_ = play::json_reader::read(get_json(), "address", "127.0.0.1:7000");
}

}  // namespace alpha