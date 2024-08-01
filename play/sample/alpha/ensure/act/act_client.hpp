#pragma once

#include <alpha/ensure/act/client.hpp>
#include <play/app/act.hpp>

namespace alpha {

// bot 액터의 컴포넌트로 사용할 클라이언트 wrapper
/**
 * 봇별로 클라이언트 연결과 frame_handler가 필요하여 컴포넌트로 처리
 */
class act_client : public play::act
{
public:
  act_client(play::actor& owner);

  client::frame_handler& get_handler()
  {
    return handler_;
  }

  client& get_client()
  {
    return client_;
  }

private:
  client::frame_handler handler_;  // 자체 핸들러가 필요. 봇별로 동작
  client client_;
};

}  // namespace alpha
