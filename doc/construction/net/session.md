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


