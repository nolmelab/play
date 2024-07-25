# ensure 

play가 플레이하다는 걸 확실히 하고, app 개발 시 기능과 부하 테스트 용으로 
함께 사용하기 위한 툴입니다. 

playplay 프로젝트에서 만드는 게임들은 모두 ensure로 플레이 가능하게 할 예정입니다. 
RL (Reinforcement Learning)이나 심층 학습을 통해 자동 플레이가 가증하도록 하는 것도 
꽤 오래된 꿈 중의 하나입니다. 

## 기반

play이 기능을 최대한 활용하여 서버 구현과 최대한 비슷하게 합니다. 

봇툴은 서버이자 클라입니다. 클라이언트의 서버 요청을 시뮬레이션 하지만 대규모
처리를 하므로 서버의 요구 사항을 만족해야 합니다.

asio의 io_context를 통한 비동기 실행을 잘 활용하여 타이머를 통한 스케줄링까지
활용합니다. 이는 서버에서 npc나 pc를 실행하는 방식과 거의 동일합니다. 

BT(행동 트리, BehaviorTree)를 사용하는 걸 잠시 고려했습니다. 하지만, 봇들은 
event 기반에 reactive 하게 동작하므로 HFSM이 더 잘 맞습니다. 능동적인 행동이 
별로 필요 없고 수동적으로 이벤트에 반응하기 때문입니다. 응답 성능을 
측정 할 수 있어야 한다는 점도 중요한 고려 사항입니다. 

## 구조 

- ensure app
  - has a thread_runner 
  - has bots 
    - acts 
      - HFSM based 
      - called flow (of acts)
    - memory 
      - blackboard like, but more librally used
    - app::client
      - connect to server and communicate

## 기능 

### 기반 

- ensure_app
  - 구성 json 로딩 
  - bot 생성 
  - bot별로 flow 로딩과 act 생성 



### 프로토콜 

여러 프로토콜을 지원할 수 있어야 합니다. 예를 들어, 가능하면 웹 테스트도 할 수 있고 
레디스 테스트도 할 수 있습니다. 이렇게 하려면 봇에서 관리하는 프로토콜은 특정될 수 없고 
act들에서 지정해야 합니다. 

- 다양한 프로토콜 지원 
- 기본 프로토콜로 secure_protocol에 flatbuffer::NativeTable 프레임 지원 
