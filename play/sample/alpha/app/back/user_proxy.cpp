#include <alpha/app/back/user_proxy.hpp>

namespace alpha {

void user_proxy::do_logout()
{
  // nothing to do. user_master will delete me.
}

bool user_proxy::on_start()
{
  return true;
}

void user_proxy::on_stop()
{
  //
}

}  // namespace alpha