#include <play/base/json_reader.hpp>
#include <play/ensure/act_factory.hpp>
#include <play/ensure/bot.hpp>
#include <play/ensure/ensure_app.hpp>

namespace ensure {

bot::bot(ensure_app& app, const nlohmann::json& json, const std::string& name, size_t index)
    : app_{app},
      json_{json},
      name_{name},
      index_{index}
{
}

bool bot::start()
{
  flow_ = std::make_shared<flow>(*this, json_);  // acts need to be shared_ptr always
  auto rc = flow_->activate();

  if (rc)
  {
    auto interval = play::json_reader::read(json_, "ensure.update_interval", 200);
    auto self = shared_from_this();
    bot_timer_ = app_.get_runner().repeat(get_id(), std::chrono::milliseconds(interval),
                                          [this, self](play::timer& timer)
                                          {
                                            PLAY_UNUSED(timer);
                                            this->update();
                                          });
    LOG()->info("bot: {}. timer started.", get_name());
    return true;
  }

  return false;
}

void bot::update()
{
  // act_serial의 on_update 호출
  flow_->update();
}

void bot::stop()
{
  if (bot_timer_)
  {
    bot_timer_->cancel();
  }
  flow_->deactivate();
  LOG()->info("bot: {} stopped.", get_name());
}

}  // namespace ensure