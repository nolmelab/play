# 작업 목록 

- [ ] alpha 앱의 시작
  - [ ] 골격 코드 작성 
  - [ ] server 시작 
    - [ ] flatbuffer_handler의 연결 / 단선 구독 기능 추가 

- [ ] tc_login 구현과 테스트 
  - [ ] act_connect_lobby 작성과 테스트 
    - [ ] client 보관과 사용 방법 정리 
  - [ ] act_login 작성과 테스트 
    - [ ] 대응하는 lobby_runner 기능 작성 
    - [ ] 대응하는 lobby_master 기능 작성 
    - [ ] 대응하는 actor_user_master 기능 작성 
    - [ ] 중복 로그인 처리 확인 
      - [ ] 프론트와 백엔드 각 실행 후 처리 확인 
      - [ ] remote_call_service의 구현과 동작 확인 

와! tc_login 동작이 기능의 절반이 넘습니다. 이와 같이 가장 기본적인 프로토콜의 구현이 
가장 어려운 과정이기도 합니다. 이후에는 기반 구조 상에서 프로토콜을 추가하고 핸들러들을 
작성하면 됩니다. 

- [ ] tc_chat 구현과 테스트 

따로 태스크를 적지 않아도 근본적인 기능 구현은 거의 없어 보이고 프로토콜과 
핸들러들 작성으로 잘 진행될 것으로 보입니다. 
