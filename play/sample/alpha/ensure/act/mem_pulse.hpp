#pragma once

#include <play/ensure/blackboard.hpp>
#include <play/net/protocol/secure_protocol.hpp>
#include <play/net/pulse_fb.hpp>

namespace alpha {

struct mem_pulse : public play::blackboard::mem
{
  using pulse = play::pulse_fb<play::secure_protocol<uint16_t>>;
  std::shared_ptr<pulse> ptr_;

  mem_pulse()
  {
    ptr_ = std::make_shared<pulse>();
  }

  pulse* get_pulse() 
  {
    return ptr_.get();
  }
};

}  // namespace alpha