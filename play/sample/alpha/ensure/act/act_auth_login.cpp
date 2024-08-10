#include <alpha/share/fb/auth_generated.h>
#include <alpha/share/fb/topic_generated.h>
#include <alpha/ensure/act/act_auth_login.hpp>
#include <play/base/json_reader.hpp>
#include <play/ensure/bot.hpp>

namespace alpha {

bool act_auth_login::on_activate()
{
  return true;
}

void act_auth_login::on_deactivate() {}

}  // namespace alpha