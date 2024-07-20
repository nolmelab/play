# session 

a wrapper for socket and asio async operations. 

## responsibility 

- sends bytes 
- recv bytes 
- notifies socket (session) state to session_handlers
  - client and server are those session_handlers 
- provides functions to send frames 

- protocol provides application functionality, but session does not 
  - session, client, server are intended for reuse 
  - protocol is intended to provide application specific needs

## relation with session_handler 

server와 client의 기반 클래스로 session_handler는 의미가 있다. 
따라서, 그대로 둔다. 대신 adapter를 통해서 필요한 함수들을 제공하는 구조를 사용한다. 

상속을 통한 인터페이스 제공 대신에 인터페이스 타잎을 제공하는 방법이고 
이렇게 분리하면 어디서나 사용할 수 있게 된다. 

```c++
struct test_adapter : session::protocol_adater
{

}
```

위와 같은 상속으로 기본 어댑터를 활용하면서 필요한 기능을 추가할 수 있습니다. 
session_handler를 통한 virtual 함수로 프레임을 받는 것이 마음에 들지 않으면 직접 
앱에서 받도록 함수를 제공할 수 있습니다. 

## protocol 호출 

session에서 accepted / connected / closed를 호출합니다. 


## close / closed 처리 

통신에서 단선은 연결만큼 중요한 사건입니다. 연결에 대한 종료를 어떻게 처리해야 할지 
여전히 어렵습니다.

secure_protocol을 예로 들어 살펴보겠습니다. 

### server<secure_protocol>

  - case 1. 협상 완료 전 상대방이 close 
    - start_recv() 중 
      - handle_recv()에서 통지
    - start_send() 중  
      - handle_send()에서 통지. 중복 통지 가능 . is_open()일 때만 통지
      - 앱에서는 해당 세션을 모를 수 있음
    - handle_recv() 중 
      - start_recv()와 다음 handle_recv()에서 통지 
    - handle_send() 중 
      - 인지 불가. 통지 안 됨
     


  - case 2. 협상 완료 후 상대방이 close

    - case 2.1 협상 완료 통지 전 

    - case 2.2 협상 완료 통지 후 

  - case 3. 협상 후 내가 close

    
### 처리 정리 

- closed()는 한번만 통지됨 
- 협상 여부와 무관하게 항상 통지 
  - 앱에서 구분하여 처리할 필요 
  - on_established()가 호출된 세션과 아닌 세션으로 나뉨
- 앱에서 close() 해도 끊어진 세션과 동일하게 처리 
  - 앱 프로토콜 

명확하고 정확하면 대응하여 처리를 구현할 수 있습니다. 












