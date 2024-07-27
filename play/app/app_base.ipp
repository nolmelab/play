#include <play/app/app_base.hpp>
#include <play/base/logger.hpp>

namespace play {

template <typename Protocol, typename Frame>
template <typename Service, typename... Args>
bool app_base<Protocol, Frame>::create_service(Args&&... args)
{
  auto service = std::make_shared<Service>(*this, std::forward(args)...);
  auto index = std::type_index{typeid(Service)};
  auto result = services_.insert(std::pair{index, service});

  if (!result.second)
  {
    LOG()->warn("service exists. type_name: {}", index.name());
  }
  return result.second;
}

template <typename Protocol, typename Frame>
template <typename Service>
inline std::shared_ptr<Service> app_base<Protocol, Frame>::get_service()
{
  auto index = std::type_index{typeid(Service)};
  auto iter = services_.find(index);
  if (iter == services_.end())
  {
    return {};
  }
  return iter->second;
}

template <typename Protocol, typename Frame>
void app_base<Protocol, Frame>::for_each(std::function<void(service::ptr)> fn)
{
  for (auto& kv : services_)
  {
    fn(kv.second);
  }
}

}  // namespace play