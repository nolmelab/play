#include <alpha/app/app.hpp>
#include <alpha/app/back/room_master.hpp>
#include <alpha/app/back/user_master.hpp>
#include <alpha/app/front/lobby_runner.hpp>
#include <alpha/app/front/room_runner.hpp>
#include <fstream>
#include <play/base/json_reader.hpp>

namespace alpha {

bool app::start(const std::string& config_file)
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

bool app::start(const nlohmann::json& jconf)
{
  jconf_ = jconf;
  stop_ = false;

  try
  {
    auto default_concurrency = std::thread::hardware_concurrency();
    auto concurrency = play::json_reader::read(jconf_, "alpha.concurrency", default_concurrency);
    auto port = play::json_reader::read(jconf_, "alpha.port", 7000);
    role_ = play::json_reader::read(jconf_, "alpha.role", std::string{"back"});  // or front

    LOG()->info("starting app with role: {}", role_);

    runner_ = std::make_unique<play::thread_runner>(concurrency, "app");
    auto rc_1 = pulse_.as_server(runner_.get(), port).start();
    if (!rc_1)
    {
      return false;
    }

    if (role_ == "front")
    {
      auto addr = play::json_reader::read(jconf_, "alpha.backend", std::string{"127.0.0.1:7000"});

      pulse_back_.as_client(runner_.get(), addr).sub(pulse::topic_closed, PULSE_FN(on_closed_back));
      // pulse_back_의 start()는 서비스 start 이후로 하여 established를 받을 수 있도록 함
    }

    auto rc_2 = start_services();
    if (rc_2)
    {
      LOG()->info("started app");
    }
    return rc_2;
  }
  catch (std::exception& ex)
  {
    LOG()->error("exception: {} while starting ensure_app", ex.what());
    return false;
  }
}

void app::wait()
{
  while (!stop_)
  {
    play::runner::sleep(100);
  }
}

void app::stop()
{
  if (stop_)
  {
    return;
  }

  stop_ = true;
  runner_->stop();
  pulse_.stop();

  stop_services();
}

bool app::start_services()
{
  // be or fe에 따라 필요한 서비스를 생성한다.
  if (role_ == "front")
  {
    create_service<lobby_runner>(*this);
    get_service<lobby_runner>()->start();

    create_service<room_runner>(*this);
    get_service<room_runner>()->start();

    return pulse_back_.start();
  }
  // else
  PLAY_CHECK(role_ == "back");

  create_service<user_master>(*this);
  get_service<user_master>()->start();

  create_service<room_master>(*this);
  get_service<room_master>()->start();

  return true;
}

void app::stop_services()
{
  for_each(
      [](auto service)
      {
        service->stop();
      });
}

void app::on_closed_back(pulse::session_ptr se, pulse::frame_ptr fr)
{
  pulse_back_.connect();
}

}  // namespace alpha
