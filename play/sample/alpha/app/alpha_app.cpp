#include <alpha/app/alpha_app.hpp>
#include <fstream>
#include <play/base/json_reader.hpp>

namespace alpha {

bool alpha_app::start(const std::string& config_file)
{
  config_file_ = config_file;

  try
  {
    nlohmann::json jconf;
    std::ifstream ifs(config_file);
    ifs >> jconf;

    return start(jconf);
  }
  catch (std::exception& ex)
  {
    LOG()->error("exception: {} while starting ensure_app", ex.what());
    return false;
  }
}

bool alpha_app::start(const nlohmann::json& jconf)
{
  jconf_ = jconf;
  stop_ = false;

  try
  {
    auto default_concurrency = std::thread::hardware_concurrency();
    auto concurrency = play::json_reader::read(jconf_, "alpha.concurrency", default_concurrency);
    auto port = play::json_reader::read(jconf_, "alpha.port", 7000);
    role_ = play::json_reader::read(jconf_, "alpha.role", std::string{"back"});  // or front

    LOG()->info("starting alpha_app with role: {}", role_);

    runner_ = std::make_unique<play::thread_runner>(concurrency, "alpha_app");
    handler_ = std::make_unique<frame_handler>(*runner_);
    server_ = std::make_unique<server>(*handler_);

    server_->start(port);

    auto rc = start_services();
    if (rc)
    {
      LOG()->info("started alpha_app");
    }
    return rc;
  }
  catch (std::exception& ex)
  {
    LOG()->error("exception: {} while starting ensure_app", ex.what());
    return false;
  }
}

void alpha_app::wait()
{
  while (!stop_)
  {
    play::runner::sleep(100);
  }
}

void alpha_app::stop()
{
  if (stop_)
  {
    return;
  }

  stop_ = true;
  runner_->stop();
  server_->stop();

  stop_services();
}

bool alpha_app::start_services()
{
  // be or fe에 따라 필요한 서비스를 생성한다.
  if (role_ == "front")
  {
    // lobby_runner, room_runner
  }
  else
  {
    PLAY_CHECK(role_ == "back");
    // user_master, room_master
  }

  return true;
}

void alpha_app::stop_services()
{
  for_each(
      [](play::service::ptr service)
      {
        service->stop();
      });
}

}  // namespace alpha
