#include <play/robust/base/macros.hpp>
#include <play/robust/ensure/act_factory.hpp>

namespace play { namespace robust { namespace ensure {

act_factory& act_factory::get()
{
  static act_factory inst_;
  return inst_;
}

act::ptr act_factory::create(const std::string& type, app::actor& owner, act::ptr parent,
                             const nlohmann::json& json, const std::string& name)
{
  auto iter = creators_.find(std::string{type});
  if (iter == creators_.end())
  {
    return {};
  }
  return iter->second(owner, parent, json, name);
}

void act_factory::reg(std::string_view type, creator fn)
{
  auto result = creators_.insert(std::pair{type, fn});
  PLAY_CHECK(result.second);
}

}}}  // namespace play::robust::ensure