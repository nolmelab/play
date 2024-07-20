# ensure 

robust가 로버스트하다는 걸 확실히 하고, app 개발 시 기능과 부하 테스트 용으로 
함께 사용하기 위한 툴입니다. 

playrobust 프로젝트에서 만드는 게임들은 모두 ensure로 플레이 가능하게 할 예정입니다. 
RL (Reinforcement Learning)이나 심층 학습을 통해 자동 플레이가 가증하도록 하는 것도 
꽤 오래된 꿈 중의 하나입니다. 

## 기반

robust이 기능을 최대한 활용하여 서버 구현과 최대한 비슷하게 합니다. 

봇툴은 서버이자 클라입니다. 클라이언트의 서버 요청을 시뮬레이션 하지만 대규모
처리를 하므로 서버의 요구 사항을 만족해야 합니다.

asio의 io_context를 통한 비동기 실행을 잘 활용하여 타이머를 통한 스케줄링까지
활용합니다. 이는 서버에서 npc나 pc를 실행하는 방식과 거의 동일합니다. 

전에는 HFSM 기반으로 봇 행동을 구현했으나 이번에는 행동 트리 (Behavior Tree)를
활용하려고 합니다. 원래 손쉽고 직관적인 행동의 구성(Composition)이 목적이므로
봇툴과 잘 맞을 것으로 예상합니다. 성능 측정이 아닌 행동의 구현과 부하 발생이
목적이기 때문에 더 잘 맞습니다. 성능 측정은 HSFM이 더 수월합니다. 

## 구조 

- ensure app
  - has a thread_runner 
  - has bots 
    - has a behaivor tree configured 
    - acts 
      - behaivor tree actions, decorators to run tests
    - memory 
      - blackboard like, but more librally used
    - app::client
      - connect to server and communicate

behaviortree와 이벤트의 결합을 어떻게 해야할지 아직 명확하지 않습니다. 
보통 명확하지 않은 부분이 핵심 중 하나입니다. 다른 부분은 중요하더라도 해결된 
문제일 가능성이 많습니다.










