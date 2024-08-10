#pragma once

#include <play/net/session_handler.hpp>

namespace play {

// 세션 핸들러와 동일한 정보를 펄스로 받기위한 인터페이스
/**
 * pulse에서만 Frame을 알고 server/client/session 등에는 숨김 
 * pulse_listener란 이름으로 펄스 연결을 알려줌
 */
template <typename Session>
class pulse_listener : public session_handler<Session>
{
};

}  // namespace play