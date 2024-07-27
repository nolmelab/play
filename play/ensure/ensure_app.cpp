#include <fstream>
#include <play/base/json_reader.hpp>
#include <play/ensure/act_factory.hpp>
#include <play/ensure/acts/act_message.hpp>
#include <play/ensure/acts/act_wait_duration.hpp>
#include <play/ensure/ensure_app.hpp>

namespace ensure {

bool ensure_app::start(const std::string& config_file)
{
  config_file_ = config_file;

  try
  {
    nlohmann::json jconf;
    std::ifstream ifs("file.json");
    ifs >> jconf;

    return start(jconf);
  }
  catch (std::exception& ex)
  {
    LOG()->error("exception: {} while starting ensure_app", ex.what());
    return false;
  }
}

bool ensure_app::start(const nlohmann::json& jconf)
{
  stop_ = false;
  jconf_ = jconf;

  try
  {
    auto default_concurrency = std::thread::hardware_concurrency();
    auto concurrency =
        play::json_reader::read(jconf_, "ensure_app.concurrency", default_concurrency);
    auto port = play::json_reader::read(jconf_, "ensure_app.port", 7000);

    register_default_acts();

    runner_ = std::make_unique<play::thread_runner>(concurrency, "ensure_app");
    handler_ = std::make_unique<frame_handler>(*runner_);
    server_ = std::make_unique<server>(*handler_);

    server_->start(port);

    bot_count_ = play::json_reader::read(jconf_, "ensure_app.bot_count", 1);
    bot_start_index_ = play::json_reader::read(jconf_, "ensure_app.bot_start_index", 1);
    bot_prefix_ = play::json_reader::read(jconf_, "ensure_app.bot_prefix", std::string{"bot_"});

    start_bots();

    return true;
  }
  catch (std::exception& ex)
  {
    LOG()->error("exception: {} while starting ensure_app", ex.what());
    return false;
  }
}

void ensure_app::wait()
{
  while (!stop_)
  {
    play::runner::sleep(100);
  }
}

void ensure_app::stop()
{
  if (stop_)
  {
    return;
  }

  stop_ = true;
  runner_->stop();
  server_->stop();
  stop_bots();
}

bool ensure_app::start_bots()
{
  for (size_t i = 0; i < bot_count_; ++i)
  {
    auto index = bot_start_index_ + i;
    auto bp = std::make_shared<bot>(*this, jconf_, fmt::format("{}{}", bot_prefix_, index), index);
    auto rc = bp->start();
    if (rc)
    {
      bots_.push_back(bp);
    }
  }

  return true;
}

void ensure_app::stop_bots()
{
  for (auto& bp : bots_)
  {
    bp->stop();
  }
}

void ensure_app::register_default_acts()
{
  PLAY_REGISTER_ACT(act_message);
  PLAY_REGISTER_ACT(act_wait_duration);
}

}  // namespace ensure
