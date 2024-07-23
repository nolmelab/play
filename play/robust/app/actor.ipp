#include <play/robust/app/actor.hpp>
#include <play/robust/base/logger.hpp>
#include <play/robust/base/macros.hpp>
#include <play/robust/base/dynamic_snowflake.hpp>
#include <typeindex>

namespace play { namespace robust { namespace app {

inline actor::actor() 
{
  id_ = base::dynamic_snowflake::get().next();
}

template <typename Act, typename... Args>
inline bool actor::create_act(Args&&... args)
{
  auto act = std::make_shared<Act>(*this, std::forward(args)...);
  auto index = std::type_index{typeid(Act)};
  auto result = acts_.insert(std::pair{index, act});

  if (!result.second)
  {
    LOG()->warn("act exists. type_name: {}", index.name());
  }
  else 
  {
    on_created_act(index, act);
  }

  return result.second;
}

template <typename Act>
inline std::shared_ptr<Act> actor::get_act()
{
  auto index = std::type_index{typeid(Act)};
  auto iter = acts_.find(index);
  if (iter == acts_.end())
  {
    return {};
  }
  return std::static_pointer_cast<Act>(iter->second);
}

template <typename Act>
inline bool actor::has_act()
{
  auto index = std::type_index{typeid(Act)};
  auto iter = acts_.find(index);
  return iter != acts_.end();
}

inline void actor::on_created_act(const std::type_index& type_index, act::ptr ap)
{
  PLAY_UNUSED(type_index);
  PLAY_UNUSED(ap);
}

}}}  // namespace play::robust::app
