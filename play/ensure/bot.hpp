#pragma once

#include <nlohmann/json.hpp>
#include <play/app/actor.hpp>
#include <play/ensure/blackboard.hpp>
#include <play/ensure/flow.hpp>
#include <play/net/runner/timer_service.hpp>

namespace play {

class ensure;

class bot : public play::actor
{
public:
  using ptr = std::shared_ptr<bot>;  // override actor::ptr

public:
  bot(ensure& app, const nlohmann::json& json, const std::string& name, size_t index);

  // bot timer를 통해 flow update
  void update();

  ensure& get_app()
  {
    return app_;
  }

  const ensure& get_app() const
  {
    return app_;
  }

  const std::string& get_name() const
  {
    return name_;
  }

  size_t get_index() const
  {
    return index_;
  }

  flow& get_flow()
  {
    return *flow_;
  }

  blackboard& get_blackboard()
  {
    return board_;
  }

  const blackboard& get_blackboard() const
  {
    return board_;
  }

private:
  // flow를 로딩. flow를 시작.
  bool on_start() final;

  void on_stop() final;

private:
  ensure& app_;
  nlohmann::json json_;
  std::string name_;
  size_t index_;
  std::shared_ptr<flow> flow_;
  play::timer::ref bot_timer_;
  blackboard board_;
};

}  // namespace play