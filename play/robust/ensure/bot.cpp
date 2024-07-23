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
  // create a flow
  // - create acts
  // activate the flow
  // 
}

void bot::stop()
{
  // deactivate the flow
}

}}}  // namespace play::robust::ensure