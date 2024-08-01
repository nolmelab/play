#include <alpha/ensure/ensure_alpha.hpp>

namespace alpha {

ensure_alpha& ensure_alpha::get()
{
  static ensure_alpha app_;
  return app_;
}

}  // namespace alpha