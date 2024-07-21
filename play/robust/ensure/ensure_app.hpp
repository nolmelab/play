#pragma once

#include <play/robust/app/app.hpp>

namespace play { namespace robust { namespace ensure {

// ensure app. flow를 시작. 그러면 전체 테스트가 동작
/**
 *  
 */
class ensure_app : public app::app
{
public:
  ensure_app();

  bool start(std::string_view jconf) override;

  void stop() override;

private:
  // flow를 생성하여 봇들을 실행
  bool on_start() final;

  void on_stop() final;

private:
  std::string sflow_;
  nlohmann::json jflow_;
};

}}}  // namespace play::robust::ensure