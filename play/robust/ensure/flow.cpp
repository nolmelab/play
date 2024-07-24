#include <play/robust/base/json_reader.hpp>
#include <play/robust/base/macros.hpp>
#include <play/robust/base/string_util.hpp>
#include <play/robust/ensure/act_factory.hpp>
#include <play/robust/ensure/bot.hpp>
#include <play/robust/ensure/flow.hpp>

namespace play { namespace robust { namespace ensure {

flow::flow(app::actor& bt, const nlohmann::json& json)
    : act_serial(bt, ptr{}, json, std::string{"flow"})
{
}

bool flow::on_activate()
{
  if (load_acts())
  {
    PLAY_CHECK(get_current_act() == get_acts()[0]);
    return get_current_act()->activate();
  }

  return false;
}

void flow::on_deactivate()
{
  get_current_act()->deactivate();
}

void flow::exit()
{
  get_owner().stop();
}

}}}  // namespace play::robust::ensure