#include <play/base/json_reader.hpp>
#include <play/ensure/act_factory.hpp>
#include <play/ensure/bot.hpp>

namespace play { namespace ensure {

bot::bot(ensure& app, const nlohmann::json& json, const std::string& name, size_t index)
    : app_{app},
      json_{json},
      name_{name},
      index_{index}
{
}

bool bot::start()
{
  flow_ = std::make_unique<flow>(*this, json_["flow"]);
  auto rc = flow_->activate();
  if (rc)
  {
    LOG()->info("activated the flow. bot: {} started.", get_name());
    return true;
  }
  return false;
}

void bot::stop()
{
  flow_->deactivate();
  LOG()->info("bot: {} stopped.", get_name());
}

}}  // namespace play::ensure