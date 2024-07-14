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
