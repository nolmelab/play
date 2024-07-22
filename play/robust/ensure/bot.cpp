#include <play/robust/ensure/bot.hpp>

namespace play { namespace robust { namespace ensure {

bot::bot(ensure& app, const nlohmann::json& json, const std::string& name, size_t index)
    : app_{app},
      json_{json},
      name_{name},
      index_{index}
{
}

bool bot::activate()
{
  // create a flow
}

void bot::deactivate()
{
  //
}

void bot::destroy()
{
  //
}

}}}  // namespace play::robust::ensure