# db process chain 

db.md 문서에서 간략하게 게임 서버에서 취하고 있는 처리 사슬에 대해 살펴보았습니다. 
여러 가지 선택이 있고 처리 모델과 맞물립니다. c#이나 node.js 같은 경우는 async / await로 
처리 단위별로 비동기 처리를 코루틴 (실제로는 상태 기계) 형식으로 이어서 처리하는 
구조로 구현합니다. 

c++에서도 db 호출을 다른 쓰레드로 넘기고 (post나 future 등) 이를 co_wait로 대기해서 
이어서 처리한다면 완료 코드를 따로 분리할 필요가 없으므로 좀 더 편리해질 듯 합니다. 

현재로서는 io_context 기반의 비동기 콜백 형식의 사슬이 가장 익숙하고 편리합니다. 
별도의 서버로 DB 처리를 분리하고 패킷으로 콜백 처리를 하는 경우입니다. 장비가 분리되고 
패킷이 비동기 호출을 자동으로 만든다는 점만 제하면 io_context와 거의 유사합니다. 

## 모델 

- persistent_property 
  - lock(), unlock(), is_locked() const interface

- db_runner 
  - 별도의 thread_runner
  - 쓰레드 개수가 많음 (수 백 ~ 수 천)
  - post(actor, persistent_property&, functor)

- actor 
  - 게임 로직의 구현 단위 
  - controller를 컴포넌트로 가짐 
  - controller::on_req_add_item()이 최종 처리 위치 
    - on_db_add_item()을 완료 함수로 제공
    - db 처리 함수는 별도로 분리 

- item_db
  - add_item(actor, prop, ...) 
  - del_item()
  - get_inven()
  - get_item() 
  - ....
 
- user_db
- pc_db 
- guild_db

db 처리를 postgresql로 연습할 때 위와 같이 구분하여 테이블을 설계하고 sp를 작성하여 
잘 동작하는지 확인합니다. 

