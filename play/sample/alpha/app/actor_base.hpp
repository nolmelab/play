#pragma once

#include <alpha/app/alpha_app.hpp>
#include <play/app/actor.hpp>

namespace alpha {

// 알파 액터의 기반 클래스
class actor_base : public play::actor
{
public:
  using frame_ptr = std::shared_ptr<alpha_app::frame>;
  using session_ptr = std::shared_ptr<alpha_app::session>;

public:
};

}  // namespace alpha
