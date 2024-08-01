#pragma once

#include <alpha/ensure/act/act_client.hpp>

namespace alpha {

// component 타잎을 제공하여 다른 클라이언트와 분리
class act_client_lobby : public act_client
{
public:
  act_client_lobby(play::actor& owner)
      : act_client{owner}
  {
  }
};

}  // namespace alpha