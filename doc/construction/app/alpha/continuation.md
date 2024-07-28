# continuation aka. 이어서 처리하기 

## rcs (remote_call_service)

rcs가 핵심 기능 중 하나입니다. 분산 처리에서 항상 응답이 온다는 보증이 
있는 호출은 앱 기능 작성을 편하게 합니다. 에러 처리도 같은 곳에서 할 수 있기 때문입니다. 

rcs::get_remote("").create(topic, frame_ptr, strand).then().fail().call(); 
rcs::get_remote("").create(topic, frame_ptr).then().fail().call(); 

위와 같은 아이디어를 node.js에서 많이 봤습니다. 비동기 처리를 이어가기위한 인터페이스입니다. 
play::play의 통신 기능의 최종 인터페이스인 flabuffer_handler와 연계하여 
어떻게 구조를 만들 수 있을까요? 


remote_site와 같은 오브젝트에서 호출 아이디를 관리해야 합니다. uuid를 키로 하여 
믿고 쓸 수 있게 하는 것이 좋겠습니다. 

- call() 
  - remote_site에 해당 명령 등록 
  - flatbuffer_handler에 topic 구독 
    - remote_call_service 시작 시 등록 
    - master_call_service와 같은 하위 클래스에서 처리 
    - 해당 topic이 오면 해당 topic에 대한 call 있는지 확인 
    - 있으면 call의 then() 호출 
  - 단선 통지 되면 해당 remote_site의 모든 call에 대해 fail 호출 
  - 주기적인 타이머에서 타임아웃 체크 
    - 타임아웃 발생시 fail() 호출 

actor 수준에서도 호출 가능해야 하고 안정적이어야 합니다. 
- shared_ptr 캡처로 안전하게 할 수 있습니다. 
- 실행 문맥이 actor의 실행 문맥과 동일해야 합니다. 
- strand 호출과 아닌 경우를 분리하면 됩니다. 

앱에서 적절히 편하게 쓸 수 있도록 하고 RPC 구현 방향으로 가지는 않습니다. 
play::play의 통신 기능을 충분히 사용하고 flatbuffer_handler의 기능을 
활용하기위해 메세지 정의에 rpc 정보를 일부 (호출 ID)를 포함해서 구현합니다. 


### timeout 지원 관련 

요청은 보냈는데 응답이 일정 시간 없을 경우 타임아웃 처리를 하게 되면 두 시스템 간의 
상태가 달라지는 문제가 생길 수 있습니다. 

따라서, 타임아웃은 지원하지 않습니다. 

### remote_call_service의 구현 단위 

세션 수준에서 생각하는 것이 좋을 듯 합니다. accept 한 쪽이건 connect 한 쪽이건 rcs를 
사용할 수 있어야 합니다. 구독을 세션 / 토픽 단위로 해서 관심 범위를 줄일 수 있습니다. 

syn_rcs_up 토픽에 대한 처리를 하면서 세션을 획득합니다. 얻은 세션들에 대해 해당 
이름으로 call 할 수 있습니다. 

### 진행 

테스트를 하면서 지속적으로 개선하는 것이 좋습니다. 골격을 만들고 프로토콜을 하나씩 
추가하면서 필요한 지점에 넣습니다. 

이제 alpha의 서버를 빌드하고 flatbuffer 프로토콜을 추가하기 직전에 도달했습니다. 
rcs를 포함한 전체적인 메세지 처리 흐름에 대해 정리하고 있습니다. 

### auth_req_login_f2b와 auth_res_login_b2f 이어 처리하기

  - back session을 알고 있음
  - keep auth_req_login_f2b 
  - flatbuffer_handler에 미리 구독 신청 
    - rcs를 사용할 프로토콜들만 
    - session_state도 구독 
  - 세션에 있으면 될 것 같은데... 
    - 세션을 상속하는 구조로 세션 내에 처리를 연결하는 방법도 가능하다. 
    - 지금 선택은 indirection을 두고 외부에서 inject 하는 방법
  - rcs::bind_topic( req, res ); 
    - flatbuffer_handler::sub_topic( res, on_rcs_reply) 
    - flatbuffer_handler::sub_session_state( res, on_session_state )
  - rcs::start()
    - flatbuffer_handler::sub_topic( rcs_syn_up )
  - rcs::call<Msg>("name", actor::ptr, topic, Msg& m, cb)
    - calls_
    
위에서 정리한 흐름으로 보면 구체적인 구현은 달라질 수 있겠으나 호출을 보관하고 
frame_handler에서 통지를 받아 해당 호출에 대한 콜백을 호출해 주는 방식의 
동작은 괜찮을 것 같습니다. 


## service와 actor의 메세지 전달과 continuation 

lobby_runner에서 처리할 여러 이벤트 중 사용자의 생성과 소멸에 해당하는 login과 logout은 
lobby_runner 서비스에서 처리가 적당합니다. 그 외 room 관련 메세지들은 actor_user에서 
처리하면 되고 관심 범위가 좁아지므로 구현이나 유지보수가 더 수월해집니다. 

frame_handler는 세션 단위로 구독과 배포하는 기능이 없으므로 ! 이 기능을 서비스 단에서 
처리해줘야 합니다. 

어딘가에서 actor_user, 세션, 토픽이 연결된다는 점을 보관하고 전달해줘야 하는데 
lobby_runner는 이를 이미 잘 알고 있습니다. 그러므로 가장 명확하게 처리할 수 있는 지점입니다. 

frame_handler에 토픽 구독을 하면서, 전달 함수를 등록합니다. 전달 함수는 세션에 해당하는 
actor_user를 찾아서 post(session, frame)을 actor_user에 대해 호출합니다. 

post() 이후는 actor_user 내에서 배포하고 처리합니다. 

이를 위해 간단한 frame_dispather<Session, Frame>을 만들어서 필요한 곳에서 상속하거나 
포함해서 쓸 수 있게 합니다. 

이와 같은 여러 단계 전달 과정이 비효율적이지는 않을까요? 편리하고 안전하다면 std::function() 
호출과 map의 O(1) find 정도의 부하는 충분히 가치가 있습니다. 

