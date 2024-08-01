#pragma once

#include <play/ensure/ensure_app.hpp>

namespace alpha {

class ensure_alpha : public ensure::ensure_app
{
public:
  static ensure_alpha& get();
};

}  // namespace alpha