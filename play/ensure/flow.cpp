#include <play/base/json_reader.hpp>
#include <play/base/macros.hpp>
#include <play/base/string_util.hpp>
#include <play/ensure/act_factory.hpp>
#include <play/ensure/bot.hpp>
#include <play/ensure/flow.hpp>

namespace play {

flow::flow(play::actor& bt, const nlohmann::json& json)
    : act_serial(bt, ptr{}, json, std::string{"flow"})
{
}

void flow::exit()
{
  get_owner().stop();
}

}  // namespace play
