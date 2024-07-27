#include <play/base/json_reader.hpp>
#include <play/base/logger.hpp>
#include <play/ensure/acts/act_wait_duration.hpp>

namespace ensure {

act_wait_duration::act_wait_duration(play::actor& owner, act::ptr parent,
                                     const nlohmann::json& json, const std::string& name)
    : act(owner, parent, json, name)
{
}

void act_wait_duration::on_update()
{
  if (get_stop_watch().get_elapsed() >= duration_)
  {
    succed("duration passed");
  }
}

void act_wait_duration::on_load_json()
{
  duration_ = play::json_reader::read(get_json(), "duration", 0);
}

}  // namespace ensure