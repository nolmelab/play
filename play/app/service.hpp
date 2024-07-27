#pragma once

#include <play/app/actor.hpp>

namespace play {

// service들의 기반 클래스
class service : public actor
{
public:
  using ptr = std::shared_ptr<service>;

public:
  service() = default;

  virtual bool start() = 0;

  virtual void stop() = 0;
};

}  // namespace play