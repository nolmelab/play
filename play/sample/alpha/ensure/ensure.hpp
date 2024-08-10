#pragma once

#include <play/ensure/ensure.hpp>

namespace alpha {

class ensure : public play::ensure
{
public:
  static ensure& get();
};

}  // namespace alpha