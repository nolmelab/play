# db 

nanodbc를 사용한다는 점 외에 다른 기능을 추가할 필요가 있을까요? 

먼저 db 전용 runner를 통해 db 호출을 처리할 필요가 있습니다. 왜냐하면 앱이 전용 쓰레드를 
갖고 실행되는 경우가 많은 요청을 하나의 스트랜드에서 처리하는 경우 블록킹 호출을 
별도 runner로 분리할 필요가 있기 때문입니다. 

- db_runner와 다른 runner 전달 방법 
  - 전달과 실행 로직을 포함해서 호출 하는 방법
  - 또는 다른 편의 기능 

db 연결 풀 기능이 필요합니다. db_pool::get("name") 또는 db_pool::get()으로 사전에 
설정된 풀에서 연결을 가져오고 돌려주는 방법이 필요합니다. object_pool에서 사용한 
shared_ptr의 custom deleter를 할용하면 되겠습니다. 

nanodbc를 쓸 때 문자열 바인딩이나 다국어 지원 등에서 헷갈리는 경우가 있습니다. 
또 tinyint를 직접 지원하지 않기도 합니다. 이와 같은 부분들을 깔끔하게 정리한 
코드를 보여주는 것도 필요합니다. 

## postgresql 

postgresql 기반으로 게임 DB 모델을 간단하게 정리하고 동작하는 예시 코드를 
test로 만듭니다. db 프로젝트 내에 직접 넣도록 합니다. 

property 개념은 여전히 계속 떠오릅니다. 아마도 유용한 개념일 가능성이 높습니다. 
serialization 기능을 넣으면 이어서 처리하기를 원격에 대해서도 할 수 있고 
레디스 캐싱도 수월해집니다. DB 처리의 단위로 전달할 때도 유용해 보입니다. 

property에 seriailization 기능이 있다면 복사도 간단하게 됩니다. 

db sp 처리 함수의 데이터 단위가 property라면 앱에 db 간접층을 간단하고 
명확하게 붙일 수 있습니다. 

controller(component), property, element 가 있습니다. property는 static, dynamic, persistent로 
나뉘고 db와는 persistent property로 대화합니다. 이렇게 게임의 데이터 단위가 property와 
element로 나뉠 수 있으면 개념적으로 명확합니다. 코드 구조화도 개념을 따라 가능할지는 
불확실합니다. 

## persistent property + sp 

persistent property는 sp 처리와 공유됩니다. 따라서, shared 특성을 갖습니다. 

shared 특성을 갖는 property는 thread-weak-safety를 만족합니다. 이는 thread-safety에서 
데이터 무결성을 보장하지만 weak safety는 크래시 없음만 만족하는 특성입니다. 

inven : persistent_propery 

- 아이템을 획득하면 인벤토리에 추가 
  - inven::add_item() 
    - 조건 체크 
    - post( call sp | complete );
      - 여기서 몇 가지 선택 사항들이 있습니다. 
      - 추가 정보를 복제할까요? 공유할까요? 
      - db 처리 완료 전 해당 항목에 대한 처리는 잠가야 합니다. 
        - 어떻게 할까요? 인벤을 잠글까요? 그러면 모든 인벤토리 함수의 시작 시 잠금 처리를 해야 합니다. 
        - 잠금은 읽기 전용일까요? 

이런 문제를 해결하기위한 한 처리 구조는 한 액터에 대한 처리를 한 스트랜드에서만 할 수 있게 하고 
db 처리도 해당 스트랜드에서 하는 것입니다. 대신 다른 쓰레드에서 해당 액터의 정보를 읽을 수는 
있게 하고 죽지 않을 정도의 약한 쓰레드 안정성을 보장합니다. 이는 고정 길이 컨테이너들만 
사용함으로써 가능합니다. 

액터별로 db post를 특정 strand로 고정합니다. 그러면 한 액터에 대한 db 처리는 모두 순서대로 
처리됩니다. 순서 처리(Sequential processing)만으로는 모든 문제가 해결되지 않습니다. 

db 처리 되는 속성 (inven, stat 등) 별로 락 기능 (처리 중이면 에러로 판단) 하는 기능이 
필요할 수도 있습니다. 메모리와 db 간 상태 일치를 위한 방법은 분산 db의 2 phase commit과 비슷한 
두 개의 저장소 간 상태 일치 문제입니다. db가 더 오래 보관하므로 db에서 먼저 처리하고 완료되면 
메모리에서 변경하는 방법이 일반적입니다. 그렇기 때문에 동일 속성 (또는 동일 오브젝트)에 대한 
락 처리를 플래그로 두는 경우가 많습니다. 

스트랜드 내에서 db까지 처리하는 모델은 자동으로 해당 액터에 대한 전체 락이 걸리는 구조라
동작합니다. 

nak 방식의 응답과 연계하여 대략 다음의 코드 모습으로 구현 가능해 보입니다. 

```c++
void on_req_add_item(item_id, ...)
{
  PLAY_CHECK_NAK_RETURN(check_cond_1());
  // ...
  PLAY_CHECK_NAK_RETURN(check_lock(inven));

  db_runner::post_locked(me, inven, func); // me : sp of the actor, inven : lockable_property
}

void on_db_add_item(...)
{
  PLAY_CHECK(!inven.is_locked());
  // 완료되면 inven은 unlock 상태.
}
```

얼마나 자동화 할 수 있는가는 해봐야 알곘으나 꽤 괜찮은 구조를 만들 수 있겠습니다. 
대부분의 게임들이 이 정도 또는 더 동시성이 낮은 구조로도 잘 동작하고 있기 때문이며 
db 처리는 안정적이고 편리하면 충분합니다. 

이와 같이 쓰레드 간, 처리 단위 모듈 간, 분산 시스템 간의 처리를 이어가는 구조를 
continuation 이라고 합니다. 우리는 처리 사슬 또는 처리 연계의 의미로 process chain이라고 
하겠습니다. 

