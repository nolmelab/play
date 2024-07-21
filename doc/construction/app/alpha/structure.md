# 구조 

- service is the core 
  - lobby_runner
  - user_master
  - room_master
  - room_runner

- remote_call_service
  - RPC와 비슷하나 비동기로 동작한다. 
  - 타임아웃 지정이 가능하고 단선시에도 통지한다. 
  - 응답이 항상 보장되는 호출 서비스이다. 
  - 취소 기능은 지원하지 않는다. 




