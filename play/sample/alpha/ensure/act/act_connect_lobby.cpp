#include <alpha/ensure/act/act_connect_lobby.hpp>
#include <alpha/ensure/ensure.hpp>
#include <play/base/json_reader.hpp>
#include <play/ensure/bot.hpp>
#include <play/net/pulse_fb.hpp>
#include <play/net/util/address.hpp>

namespace alpha {

bool act_connect_lobby::on_activate()
{
  auto pulse = std::make_shared<ensure::pulse>();
  get_bot().get_blackboard().set_ptr("lobby", pulse);

  pulse->as_client(address_)
      .with_strand(get_owner().get_id())
      .subscribe(ensure::pulse::topic_estalished,
                 [this](ensure::pulse::session_ptr se, ensure::pulse::frame_ptr fr)
                 {
                   this->succed("connected to server");
                 })
      .start();

  return true;
}

void act_connect_lobby::on_load_json()
{
  address_ = play::json_reader::read(get_json(), "address", std::string{"127.0.0.1:7000"});
}

}  // namespace alpha