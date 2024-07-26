#pragma once

#include <nlohmann/json.hpp>
#include <play/app/actor.hpp>
#include <play/ensure/flow.hpp>
#include <play/net/runner/timer_service.hpp>

namespace play { namespace ensure {

class ensure;

class bot : public app::actor
{
public:
  using ptr = std::shared_ptr<bot>;  // override actor::ptr

public:
  bot(ensure& app, const nlohmann::json& json, const std::string& name, size_t index);

  // flow를 로딩. flow를 시작.
  bool start() final;

  // bot timer를 통해 flow update
  void update();

  void stop() final;

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

private:
  ensure& app_;
  nlohmann::json json_;
  std::string name_;
  size_t index_;
  std::shared_ptr<flow> flow_;
  net::timer::ref bot_timer_;
};

}}  // namespace play::ensure