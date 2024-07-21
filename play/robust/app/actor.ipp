#include <play/robust/app/actor.hpp>
#include <play/robust/base/logger.hpp>

namespace play { namespace robust { namespace app {

template <typename Act, typename... Args>
bool actor::create_act(Args&&... args)
{
  auto act = std::make_shared<Act>(*this, std::forward(args)...);
  auto index = std::type_index{typeid(Act)};
  auto result = acts_.insert(std::pair{index, act});

  if (!result.second)
  {
    LOG()->warn("act exists. type_name: {}", index.name());
  }
  return result.second;
}

template <typename Act>
std::shared_ptr<Act> actor::get_act()
{
  auto index = std::type_index{typeid(Act)};
  auto iter = acts_.find(index);
  if (iter == acts_.end())
  {
    return {};
  }
  return std::static_pointer_cast<Act>(iter->second);
}

}}}  // namespace play::robust::app
