# remote_call_service

remote_call_service가 핵심 기능 중 하나입니다. 분산 처리에서 항상 응답이 온다는 보증이 
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


## timeout 지원 관련 

요청은 보냈는데 응답이 일정 시간 없을 경우 타임아웃 처리를 하게 되면 두 시스템 간의 
상태가 달라지는 문제가 생길 수 있습니다. 

따라서, 타임아웃은 지원하지 않습니다. 

## remote_call_service의 구현 단위 

세션 수준에서 생각하는 것이 좋을 듯 합니다. accept 한 쪽이건 connect 한 쪽이건 rcs를 
사용할 수 있어야 합니다. 구독을 세션 / 토픽 단위로 해서 관심 범위를 줄일 수 있습니다. 




