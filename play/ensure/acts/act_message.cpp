#include <play/base/json_reader.hpp>
#include <play/base/logger.hpp>
#include <play/ensure/acts/act_message.hpp>

namespace play {

act_message::act_message(play::actor& owner, act_ensure::ptr parent, const nlohmann::json& json,
                         const std::string& name)
    : act_ensure(owner, parent, json, name)
{
}

bool act_message::on_activate()
{
  auto message = play::json_reader::read(get_json(), "message", std::string{});
  LOG()->info("message from bot. {}", message);
  return true;
}

void act_message::on_update()
{
  succed("succeed after showing the message");
}

}  // namespace play