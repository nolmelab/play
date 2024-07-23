#include <play/robust/ensure/flow.hpp>

namespace play { namespace robust { namespace ensure {

flow::flow(app::actor& bt, const nlohmann::json& json)
    : act(bt, ptr{}, std::string{"flow"}),
      json_{json}
{
}

bool flow::on_activate()
{

  return true;
}

void flow::on_deactivate() {}

}}}  // namespace play::robust::ensure