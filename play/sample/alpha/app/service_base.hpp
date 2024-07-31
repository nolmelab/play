#pragma once

#include <alpha/app/alpha_app.hpp>
#include <play/app/service.hpp>

namespace alpha {

// 알파 서비스의 기반 클래스
class service_base : public play::service
{
public:
  using frame_ptr = std::shared_ptr<alpha_app::frame>;
  using session_ptr = std::shared_ptr<alpha_app::session>;
  using frame_handler = alpha_app::frame_handler;

public:
  service_base(alpha_app& app)
      : app_{app}
  {
  }

  alpha_app& get_app()
  {
    return app_;
  }

  alpha_app::frame_handler& get_handler()
  {
    return app_.get_handler();
  }

  play::runner& get_runner()
  {
    return app_.get_runner();
  }

private:
  alpha_app& app_;
};

}  // namespace alpha

// 서비스에서 topic 구독 함수 등록. strand를 기본으로 함
#define SERVICE_SUB_TOPIC(pic, func)                                                 \
  get_handler().sub_topic_strand(                                                    \
      static_cast<uint16_t>(pic),                                                    \
      [this](alpha::service_base::session_ptr se, alpha::service_base::frame_ptr fr) \
      {                                                                              \
        this->func(se, fr);                                                          \
      },                                                                             \
      get_id());

// 서비스에서 세션 상태 구독. strand를 기본으로 함
#define SERVICE_SUB_SESSION(func)                               \
  get_handler().sub_session_strand(                             \
      [this](session_ptr s, frame_handler::session_state state) \
      {                                                         \
        this->on_session_state(s, state);                       \
      },                                                        \
      get_id());
