#pragma once

#include <play/robust/app/actor.hpp>

namespace play { namespace robust { namespace app {

// system들의 기반 클래스
class system : public actor
{
public:
  system(std::string_view type_name)
      : actor(type_name)
  {
    //
  }
  `
};

}}}  // namespace play::robust::app