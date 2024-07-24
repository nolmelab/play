#pragma once

#include <play/app/actor.hpp>

namespace play { namespace app {

// service들의 기반 클래스
class service : public actor
{
public:
  using ptr = std::shared_ptr<service>;

public:
  service() = default;
};

}}  // namespace play::app