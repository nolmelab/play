#include <play/robust/base/json_reader.hpp>
#include <play/robust/base/macros.hpp>
#include <play/robust/ensure/act_factory.hpp>
#include <play/robust/ensure/flow.hpp>

namespace play { namespace robust { namespace ensure {

flow::flow(app::actor& bt, const nlohmann::json& json)
    : act(bt, ptr{}, json, std::string{"flow"}),
      json_{json}
{
}

bool flow::on_activate()
{
  current_act_ = 0;

  for (auto& jact : json_)
  {
    auto type = base::json_reader::read(jact, "type", std::string{});
    auto name = base::json_reader::read(jact, "name", std::string{});

    auto ap = act_factory::get().create(type, get_owner(), shared_from_this(), jact, name);
    PLAY_CHECK(ap);
    acts_.push_back(ap);
  }

  return get_current_act()->activate();
}

void flow::on_deactivate()
{
  get_current_act()->deactivate();
  acts_.clear();
}

void flow::signal(std::string_view sig, std::string_view message)
{
  //
}

}}}  // namespace play::robust::ensure