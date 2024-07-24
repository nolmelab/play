#include <play/base/json_reader.hpp>
#include <play/base/macros.hpp>
#include <play/base/string_util.hpp>
#include <play/ensure/act_factory.hpp>
#include <play/ensure/bot.hpp>
#include <play/ensure/flow.hpp>

namespace play { namespace ensure {

flow::flow(app::actor& bt, const nlohmann::json& json)
    : act_serial(bt, ptr{}, json, std::string{"flow"})
{
}

void flow::exit()
{
  get_owner().stop();
}

<<<<<<< HEAD:play/ensure/flow.cpp
std::pair<size_t, flow::act_ptr> flow::find_act(const std::string& name) const
{
  size_t index = 0;
  for (auto& ap : acts_)
  {
    auto eap = std::static_pointer_cast<act>(ap);
    if (eap->get_name() == name)
    {
      return {index, eap};
    }
    ++index;
  }
  return {0, {}};
}

}}  // namespace play::ensure
=======
}}}  // namespace play::robust::ensure
>>>>>>> 91c59b068a24bbbadad9bbc480851488feb7e86b:play/robust/ensure/flow.cpp
