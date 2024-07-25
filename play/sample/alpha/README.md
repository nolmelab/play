# app - alpha 

알파는 게임 기능 없이 인스턴스 내에서 채팅만 가능한 앱입니다. 

인증, 월드, 게임으로 이루어지는 플레이의 분산 처리 모델이 잘 동작하는지 검증하고 
가장 단순한 코드 기반으로 개별 게임에 맞는 기초(bare minimum)을 제공하기 위한 
프로젝트입니다. 

## 구조

- master
  - user_service
  - room_service
  - friend_service

- front 
  - auth_service
  - lobby_service
  - room_runner_service

## 기능

- 인증
- 로비 진출입
- 방 진출입
- 방 채팅
- 친구 추가 / 삭제 / 조회 
- 친구 채팅

db를 사용하지 않으면서 유용한 기능을 충분히 갖는 앱으로 적당합니다.

통신과 분산 처리 구조에 대한 사용성 검증을 충분히 하면서
ensure 봇들로 부하 테스트를 통해 성능과 안정성 검증도 할 수 있습니다.




