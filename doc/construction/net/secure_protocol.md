# secure protocol 

- 기본 프로토콜이 될 것이므로 잘 설계하고 작성해야 합니다. 
- plain_protocol로 교체를 쉽게 할 수 있어야 합니다. 
- 단위 테스트를 진행할 수 있도록 합니다. 
  - codec에서 한 실수를 다시 하지 않도록 주의합니다. 
- 단위 테스트 자체의 코드 품질도 매우 중요합니다. 


## 단위 테스트 

통신을 할 경우 send_fn() 호출이 비동기이므로 다른 쪽 엔티티와 통신 과정이 
자연스럽게 연결된다. 

단위 테스트에서 send_fn()을 어떻게 비동기로 만들 수 있을까? 
future를 사용하면 쓰레드가 된다. 

- secure_protocol p1 
  - p1.accepted()
    - sync_key()

- secure_protocol p2
  - p2.connected()
    - sync_key()

- wait on p1.is_established(), p2.is_established()

단위 테스트 진행은 항상 일정과 긴장 관계가 있다. 통신이 가능하도록 하고 
그 틀 안에서 테스트 하도록 한다. 

