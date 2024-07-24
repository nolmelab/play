#pragma once

#include <play/app/actor.hpp>

namespace play { namespace app {

// system들의 기반 클래스
class system : public actor
{
public:
  system() = default;
};

}}  // namespace play::app