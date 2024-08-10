#pragma once

#include <play/ensure/ensure.hpp>
#include <play/net/pulse_fb.hpp>

namespace alpha {

class ensure : public play::ensure
{
public:
  using pulse = play::pulse_fb<play::secure_protocol<uint16_t>>;

public:
  static ensure& get();
};

}  // namespace alpha