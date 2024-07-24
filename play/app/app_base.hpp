#pragma once

#include <play/app/service.hpp>
#include <play/net/client.hpp>
#include <play/net/frame/flatbuffer_handler.hpp>
#include <play/net/server.hpp>

namespace play { namespace app {

// app의 기반 클래스
/**
 * 서비스를 관리. 구체적인 기능은 서비스에서 구현
 */
template <typename Protocol, typename Frame>
class app_base
{
public:
  using server_type = net::server<Protocol, Frame>;
  using client_type = net::client<Protocol, Frame>;
  using session_type = typename server_type::session;

public:
  app_base() {}

  template <typename Service, typename... Args>
  bool create_service(Args&&... args);

  template <typename Service>
  std::shared_ptr<Service> get_service();

private:
  using service_map = std::map<std::type_index, service::ptr>;

private:
  service_map services_;
};

}}  // namespace play::app

#include <play/app/app_base.ipp>