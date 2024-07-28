# auth 

프로토콜 namespace로 구분한 기능 단위로 진행합니다. 

## alpha app 

알파는 secure_protocol<uint16_t, flatbuffers::NatvieTable> 을 사용하는 서버입니다. 
ensure처럼 thread_runner와 server를 갖습니다. flatbuffer_handler를 두고 각 서비스에서 
필요한 topic에 sub합니다. 

### app의 시작 

- lobby_runner 서비스 골격 작성 
- alpha.hpp와 alpha.cpp 구현 
- 서비스 생성과 시작 
- 서비스 종료
- 앱 종료 

- namespace 전체 리팩터링
  - play로 단일화 
  - play::ensure는 ensure로 변경 
  - std:: boost::asio:: 수준이 적절하다 
  - boost만큼 큰 기능 분할이 없으므로 play, ensure, alpha / beta 정도가 적절


### ensure

- ensure_alpha가 ensure_app을 상속. 내용은 거의 없이 재사용 
- ensure/act_base를 추가하여 app과 bot에 접근 가능하도록 함 
  - alpha 액트들을 기반 클래스 
  - alpha에 act_base_alpha 클래스 추가 
    - 나중에 보면 유틸리티 기능들이 필요하므로 공용 기반 클래스로 사용 

## 프로토콜 규약 

- f2b, b2f를 postfix로 추가하여 서버간 통신 명시 
  - 서버 역할이 많아져도 비슷한 두자 약어로 가능 
  - 짧고 명확해서 괜찮음 
  - f2w (front to world), f2a (front to auth)와 같은 형태
  - 분산 처리를 하면 처리해야 하는 프로토콜 수가 2배 가까이 증가. 
    - 특히, login, join 처럼 컨테이너에 대한 처리 프로토콜이 그렇다. 

### 프로로콜 검증 

- auth_req_login = 101, 
  - field: user_id  
  - lobby_runner::sub( auth_req_login, ...)
    - rcs::call( server, auth_req_login_f2b )

- auth_req_login_f2b,
  - field: rcs_id / user_id / address 

- lobby_master::sub( auth_req_login_f2b, ... )
  - authenticate (skipped at alpha)
  - create new actor_user_master
    - start it 
  - send auth_syn_login_b2f
  - rcs::reply(server, auth_res_login_b2f)  

- lobby_runner::sub( auth_syn_login_b2f, ...)
  - kickout if exists

- auth_syn_login_b2f,
  - field: user_id / result / address / etc

- auth_res_login_b2f, 
  - field: rcs_id / user_id / result  

- auth_res_login, 
  - field: result

- auth_req_logout,
- auth_req_logout_f2b, 
- auth_syn_logout_b2f, 
- auth_res_logout_b2f,
- auth_res_logout,


## rcs

rcs를 rpc와 동등한 용어로 만들고자 합니다. rcs::call(), rcs::reply()가 인터페이스입니다. 

rcs는 미리 정의된 서버들을 json 구성에서 읽어서 접속하고 로컬에만 의미있는 미리 지정된 
이름을 부여합니다. 접속을 다른 서버를 통해 받은 세션에 대해 요청할 수도 있습니다.   

- rcs::call( server, auth_req_login_f2b )
  - 이 요청은 내 노드에서 연결을 시작한 서버로 요청을 보내는 경우입니다.
  - 방 생성을 room_master에서 room_runner로 보낼 때는 rcs 처리가 필요한데 
    이 경우는 room_runner의 노드 쪽에서 맺은 연결을 사용합니다. 
  - 생각하기 좀 복잡해지는 면이 있습니다. 

- rcs_service<Protocol, Frame>
  - knows server<Protocol, Frame>
  - knows client<Protocol, Frame> optionally
  - rc_client, rcs_server는 추상화된 개념이고, session에 대해 동작한다.            

rcs_service는 call(), reply() 외에도 연결된 세션만 바로 사용하는 frame_handler의 
send와 동일한 함수도 제공합니다. 

이와 같이 복잡한 면이 있기에 게임 개발을 하면서 잘 만들기는 어려웠습니다. 이 문제를 
잘 해결하면 분산 서버에서 손쉬운 처리도 가능해 보입니다. 장애 후 안정 상태로 수렴 
가능한 구조가 그 다음 해결 과제입니다. 


### 장애 후 안정화

장애 후 안정 상태로 수렴은 장애 후 안정화 (Stabililization after Failure)로 안정화로 하겠습니다. 
예를 들어, 알파의 백엔드 서버는 한 대인데 크래시가 발생하거나 네트워크 장애가 발생한 후 
복구되면 이제 통신이 가능합니다. 이 때까지는 rcs에 온 요청은 시간이 조금 걸리더라도 모두 
실패 처리됩니다. 이후 오는 요청은 벡엔드 상태에 따라 오류로 처리하거나 현재 상태 그대로 
처리합니다. 이렇게 할 경우 더 이상 오류가 발생하지 않는 서비스 상태에 도달할까요? 

알파의 경우는 그렇다고 할 수 있습니다. 채팅은 방 단위니까 그대로 실행됩니다. 방에서 
나가기를 하면 방에서 나갈 수 있습니다. 방 진입 요청은 백엔드에서 검색부터 되지 않으므로 
들어갈 수 없습니다. 따라서, 기존 방들은 일정시간 진입이 불가능한 상태를 지나고 나면 
소멸됩니다. 

장애 범위는 call()하고 reply()를 대기하는 메세지가 얼마나 되는가입니다. 그리고, reply()가 
rcs_client 내에서 단선으로 오류처리 되는 경우 오류로 부드럽게 처리하면 됩니다. 

파티로 던전을 깨고 있고 이 파티의 구성과 위치 상태를 백엔드의 월드 서버에서 처리하고 있는 
게임을 생각해 보겠습니다. 다른 프론트에 위치한 파티원이 있으면 이 파티의 정보를 더 이상 
받을 수 없습니다. syn 패킷일 가능성이 높으니 장애로도 인지되지 않습니다. 아무것도 
하지 않는 파티원이 있는 것으로 인식됩니다. 파티원이 몇 명 없는 상태로 플레이를 지속합니다. 

월드 채팅이나 다른 월드 서버를 사용하는 서비스들에 장애가 있다는 사실을 알게 됩니다. 

그래도 중요한 던전 레이드는 마칠 수 있습니다. 그리고, 파티를 해산하려는데 오류가 나오고 
해산 처리는 해당 프론트에서 이루어집니다. 이제 월드 서버가 복구되었다는 공지가 뜹니다. 

다른 던전으로 이동을 시작하고 이 때 전달된 요청으로 빈 사용자에 대한 월드 서버 내 
복구가 이루어집니다. 이제 개인 컨텐츠들은 월드 서버 rcs 통신을 하더라도 동작하게 됩니다. 

여기서 중요한 개념인 recovery on demand가 있습니다. 요청을 처리하려고 할 때 만족하는 
조건을 충족하지 못 할 경우 장애 후 복구 상태라면 해당 정보를 복구하는 과정을 밟는다로 
요약할 수 있습니다. 

장애 후 안정화 상태 수렴을 요청 시 복구 과정을 통해 달성할 수 있다는 건 감마에서 
진행하도록 하겠습니다. 감마가 복격적인 MMORPG로 파티 플레이나 길드 플레이를 포함할 예정이기 
때문입니다. 

알파는 복구 과정이 단순하여 장애 후에도 그대로 동작할 것으로 보입니다. 그런지 ensure로 
실행하면서 확인 하겠습니다. 

### keep going 

rcs를 작은 아이디어로 시작했습니다. 점점 더 나아지고 있고 알파 구현은 rcs 구현에 가깝습니다. 

잘 마무리해야겠습니다. 끝나고 지워질 내용이겠지만. 


## lobby_runner 

- alpha::service_base의 하위 클래스 
  - frame_handler와 runner에 접근 가능. alpha_app을 통해서. 

- 역할 
  - boost::multiindex 기반의 container에 actor_user 관리 
  - on_auth_req_login 이후의 메세지들은 actor_user로 전달하여 처리 
    - 어떻게 전달할 것인가? 
    
### on_auth_req_login

- create a actor_user 
  - mark it pending users 
  - pending_users_, login_users_
  - request to back 
    - auth_req_login_f2b 
      - rcs::call()




### on_auth_req_logout



### room 메세지 처리 



