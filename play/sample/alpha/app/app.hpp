#pragma once

#include <play/app/app.hpp>
#include <play/net/protocol/secure_protocol.hpp>
#include <play/net/pulse_fb.hpp>

namespace alpha {

class app : public play::app
{
public:
  using pulse = play::pulse_fb<play::secure_protocol<uint16_t>>;

public:
  app() = default;

  // 서버 시작. 구성 로딩.
  bool start(const std::string& config_file);

  // json 구성에서 서버 시작
  bool start(const nlohmann::json& jconf);

  void wait();

  // signal로 stop
  void stop();

  pulse& get_pulse()
  {
    return pulse_;
  }

  play::runner& get_runner()
  {
    return *runner_;
  }

private:
  bool start_services();

  void stop_services();

private:
  std::string config_file_;
  nlohmann::json jconf_;
  std::unique_ptr<play::thread_runner> runner_;
  pulse pulse_;
  std::string role_;
  bool stop_{false};
};

}  // namespace alpha