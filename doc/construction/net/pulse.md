# pulse 

처음에 frame_handler로 구성했던 내용을 enusre의 act들 구현, alpha의 app 구현을 
진행하면서 일정한 패턴을 발견했고 zeromq의 소켓처럼 추상화 가능한 어떤 구조가 
잘 동작한다는 것을 알았습니다. 

이 아이디어에 바탕하여 zeromq의 socket 수준의 추상화를 진행합니다. 

## usage 

```c++
pulse<> p; 

p.as_server().with_runner(&runner).listen("0.0.0.0:7000")
.sub(session_establisehd, PULSE_SESSION_FN(on_session_established))
.sub(session_closed, PULSE_FRAME_FN(on_session_closed))
.sub(auth::req_login, PULSE_FRAME_FN(on_auth_req_login))
.start(); 
```

위의 인터페이스로 req_login까지 처리하는 흐름을 구성할 수 있습니다. 

```c++
pulse<> p; 
p.as_client().with_runner(&runner)
.sub(session_established, PULSE_SESION_FN(on_session_established))
.sub(session_closed, PULSE_FRAME_FN(on_session_closed))
.sub(auth::res_login, PULSE_FRAME_FN(on_auth_res_login))
.start();

p.connect("127.0.0.1:7000");
```

```c++
void on_session_established(pulse::session::ptr session)
{
  // create req_login
  p.send(session, req_login);
}
```

위와 같이 클라이언트로 실행하여 서버 연결이 확인되면 로그인 요청을 
보낼 수 있습니다. 

액터나 컴포넌트에서 상위 펄스가 처리하는 프레임들 중에 원하는 것을 
받아서 처리해야 할 경우가 있습니다. 코드 중복을 피하면서 효율적으로 
연결해야 합니다. 

```c++
pulse<> p; 
p.as_child(parent)
.with_runner(&runner)
.with_session(session)
.with_strand(key)
.sub(session_closed, PULSE_SESSION_FN(on_session_closed))
.sub(room::req_chat, PULSE_FRAME_FN(on_room_req_chat))
.start();
```

위와 같이 parent가 전달 가능하도록 하면서 프레임을 받도록 설정할 수 
있게 합니다. 

이와 같은 사용 예시는 현재로서는 아이디어입니다.

## 리팩터링 

- frame_handler => 제거
- flatbuffer_handler => pulse의 flatbuffers::NativeTable 특수화 

server와 client는 Frame을 모릅니다. pulse_listener를 받아서 전달할 수는 있습니다. 

- pulse_listener<Session> : session_handler<Session>
- pulse<Frame, Protocol> : pulse_listener<session<Protocol>>
  - static as_server() 
  - static as_client() 
  - static as_child()
  - with_runner()
  - with_session()
  - with_strand()
  - sub()
  - start()
  - stop()

unsub()는 필요없습니다. 이제 pulse와 구독은 함께 합니다. Topic{1}, Topic{2}를 reserve
하고 established, closed로 사용하여 별도 인터페이스를 제거합니다.    

- pulse<flatbuffers::NatvieTable, Protocol> 
  - 특수화 
  - send 
  - add_unpacker()

- template <typename Protocol> using pulse_fb = pulse<flatbuffers::NativeTable, Protocol>;

