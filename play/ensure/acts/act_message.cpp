#include <play/base/json_reader.hpp>
#include <play/base/logger.hpp>
#include <play/ensure/acts/act_message.hpp>

namespace play { namespace ensure {

act_message::act_message(app::actor& owner, act::ptr parent, const nlohmann::json& json,
                         const std::string& name)
    : act(owner, parent, json, name)
{
}

bool act_message::on_activate()
{
  auto message = base::json_reader::read(get_json(), "message", std::string{});

  LOG()->info("message from bot. {}", message);

  succed("succeed after showing the message");
  return true;
}

}}  // namespace play::ensure