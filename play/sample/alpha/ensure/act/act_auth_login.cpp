#include <alpha/ensure/act/act_uath_login.hpp>

namespace alpha {

bool act_auth_login::on_activate()
{
  // req_login() 호출

  return true;
}

void act_auth_login::on_load_json()
{
  // empty for now
}

void act_auth_login::req_login()
{
  // get connection
  // send login
}

void act_auth_login::on_res_login()
{
  // 로그인 응답 처리
}

}  // namespace alpha