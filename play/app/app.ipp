#include <play/app/app.hpp>
#include <play/base/logger.hpp>

namespace play {

template <typename Service, typename... Args>
bool app::create_service(Args&&... args)
{
  auto service = std::make_shared<Service>(std::forward<Args>(args)...);
  auto index = std::type_index{typeid(Service)};
  auto result = services_.insert(std::pair{index, service});

  if (!result.second)
  {
    LOG()->warn("service exists. type_name: {}", index.name());
  }
  return result.second;
}

template <typename Service>
inline std::shared_ptr<Service> app::get_service()
{
  auto index = std::type_index{typeid(Service)};
  auto iter = services_.find(index);
  if (iter == services_.end())
  {
    return {};
  }
  return std::static_pointer_cast<Service>(iter->second);
}

inline void app::for_each(std::function<void(service::ptr)> fn)
{
  for (auto& kv : services_)
  {
    fn(kv.second);
  }
}

}  // namespace play