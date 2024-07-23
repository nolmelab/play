#pragma once

#include <nlohmann/json.hpp>
#include <play/robust/app/actor.hpp>

namespace play { namespace robust { namespace ensure {

class ensure;

class bot : public app::actor
{
public:
  using ptr = std::shared_ptr<bot>;  // override actor::ptr

public:
  bot(ensure& app, const nlohmann::json& json, const std::string& name, size_t index);

  // flow를 로딩. flow를 시작.
  bool start() final;

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

private:
  ensure& app_;
  nlohmann::json json_;
  std::string name_;
  size_t index_;
};

}}}  // namespace play::robust::ensure