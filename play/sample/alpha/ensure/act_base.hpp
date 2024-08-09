#pragma once

#include <alpha/ensure/ensure_alpha.hpp>
#include <play/ensure/act_base.hpp>

namespace alpha {

// alpha용 act의 base 클래스. 유틸리티 함수 추가
class act_base : public ensure::act_base
{
public:
  using frame = ensure_alpha::frame;
  using session = ensure_alpha::session;
  using session_ptr = ensure_alpha::session_ptr;
  using frame_ptr = ensure_alpha::frame_ptr;
  using frame_handler = play::flatbuffer_handler<session>;

public:
  act_base(play::actor& owner, act::ptr parent, const nlohmann::json& json, const std::string& name)
      : ensure::act_base(owner, parent, json, name)
  {
  }

  ~act_base() {};
};

}  // namespace alpha

// act_base의 하위 클래스에서 토픽 구독
#define ACT_TOPIC_CB(func)             \
  [this](session_ptr se, frame_ptr fr) \
  {                                    \
    this->func(se, fr);                \
  }

// act_base의 하위 클래스에서 세션 상태 구독
#define ACT_SESSION_CB(func)                                \
  [this](session_ptr s, frame_handler::session_state state) \
  {                                                         \
    this->func(s, state);                                   \
  }
