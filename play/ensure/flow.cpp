#include <play/base/json_reader.hpp>
#include <play/base/macros.hpp>
#include <play/base/string_util.hpp>
#include <play/ensure/act_factory.hpp>
#include <play/ensure/bot.hpp>
#include <play/ensure/flow.hpp>

namespace play { namespace ensure {

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

void flow::jump(const std::string& path)
{
  auto vs = base::string_util::split(path, "/");
}

void flow::next()
{
  get_current_act()->deactivate();

  current_act_++;
  if (current_act_ >= acts_.size())
  {
    LOG()->error("flow reached the end");
  }
  else
  {
    get_current_act()->activate();
  }
}

void flow::exit()
{
  get_owner().stop();
}

std::pair<size_t, flow::act_ptr> flow::find_act(const std::string& name) const
{
  size_t index = 0;
  for (auto& ap : acts_)
  {
    auto eap = std::static_pointer_cast<act>(ap);
    if (eap->get_name() == name)
    {
      return {index, eap};
    }
    ++index;
  }
  return {0, {}};
}

}}  // namespace play::ensure