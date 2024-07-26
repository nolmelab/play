#include <play/base/json_reader.hpp>
#include <play/base/macros.hpp>
#include <play/base/string_util.hpp>
#include <play/ensure/act_factory.hpp>
#include <play/ensure/bot.hpp>
#include <play/ensure/ensure.hpp>
#include <play/ensure/flow.hpp>

namespace play { namespace ensure {

flow::flow(app::actor& bt, const nlohmann::json& json)
    : act_serial(bt, ptr{}, json, std::string{"flow"})
{
}

bool flow::start()
{
  auto rc = activate();
  if (rc)
  {
    auto interval = base::json_reader::read(get_json(), "service.update_interval", 100);
    auto me = self();
    update_timer_ =
        ensure::get().get_runner().repeat(get_owner().get_id(), std::chrono::milliseconds(interval),
                                          [this, me](net::timer& timer)
                                          {
                                            PLAY_UNUSED(timer);
                                            this->update_acts();
                                          });
  }
  return rc;
}

void flow::stop()
{
  if (update_timer_)
  {
    update_timer_->cancel();
  }
  deactivate();
}

void flow::exit()
{
  get_owner().stop();
}

void flow::update_acts()
{
  this->update();
}

}}  // namespace play::ensure
