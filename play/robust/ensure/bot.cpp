#include <play/robust/base/json_reader.hpp>
#include <play/robust/ensure/act_factory.hpp>
#include <play/robust/ensure/bot.hpp>

namespace play { namespace robust { namespace ensure {

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
  return flow_->activate();
}

void bot::stop()
{
  flow_->deactivate();
}

}}}  // namespace play::robust::ensure