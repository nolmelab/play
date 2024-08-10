#include <play/base/macros.hpp>
#include <play/ensure/act_factory.hpp>

namespace play {

act_factory& act_factory::get()
{
  static act_factory inst_;
  return inst_;
}

act_ensure::ptr act_factory::create(const std::string& type, play::actor& owner,
                                    act_ensure::ptr parent, const nlohmann::json& json,
                                    const std::string& name)
{
  auto iter = creators_.find(std::string{type});
  if (iter == creators_.end())
  {
    return {};
  }
  return iter->second(owner, parent, json, name);
}

void act_factory::add_creator(std::string_view type, creator fn)
{
  auto result = creators_.insert(std::pair{type, fn});
  if (!result.second)
  {
    LOG()->warn("act type: {} already in factory", type);
  }
}

}  // namespace play