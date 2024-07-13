#pragma once

#include <play/robust/net/asio.hpp>

namespace play { namespace robust { namespace net {

// io_context runner base class
/**
 * - io_context를 갖고 post()와 post(stran_key) 함수로 처리를 실행
 * - timer들을 갖고 스케줄링을 처리
 */
class runner
{
public:
  runner(std::string_view name = "default");

  // 가장 중요한 asio 함수. 임의의 처리를 프로액터에 맡겨서 처리.
  template <typename CompletionHandler>
  void post(CompletionHandler&& handler)
  {
    ioc_.post(std::forward<CompletionHandler>(handler));
  }

  // sleep for ms milliseconds
  void sleep(int ms);

  asio::io_context& get_ioc() { return ioc_; }

  const std::string& get_name() const { return name_; }

protected:
  asio::io_context ioc_;
  std::string name_;
};

}}}  // namespace play::robust::net