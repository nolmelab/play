#include <alpha/ensure/ensure.hpp>

namespace alpha {

ensure& ensure::get()
{
  static ensure app_;
  return app_;
}

}  // namespace alpha