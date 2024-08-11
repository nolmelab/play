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

## 1차 구현 - 2024/08/10 

기본 구현은 잘 되었습니다. pulse_fb를 구현하여 서버와 클라 실행과 통신 
테스트를 했습니다. server, client, 이전의 frame_handler를 사용할 때보다 
기능별 분리가 잘 됩니다. 특히, Frame 처리가 잘 분리되었습니다. 

개선해야할 점은 자식들에게 전파하는 과정을 효율적으로 하는 부분입니다. 
직접 토픽들을 받아서 전달하는 것이 가장 효율적입니다. 이 패턴을 
pulse 내에 구현해야 합니다. 

pulse_forwarder를 구현합니다. <topic, 자식 펄스>, <session, topic, 자식 펄스>로 
dispatch에서 전달 받을 토픽, 또는 세션과 토픽을 부모에 알려주고 받습니다. 

subscribe() 시 등록하고 unsubscribe() 시 해제합니다. unbind_child() 시에도 
해제합니다. 세션을 갖는 subscribe, unsubscribe 함수들을 등록합니다. 

루트에 등록해서 직접 전달 받습니다. 

이 외에 프레임 내용에 따라 전달해야 하는 경우도 있습니다. 월드 서버에서 
오는 인스턴스 번호를 보고 인스턴스를 찾아 전달하는 경우가 이에 해당합니다. 

이와 같은 기능은 애플리케이션에서 처리해야 합니다. 추상화 하거나 구조화 
할 수 있는 방법이 없을까요?  

또 다시 잠시 멈추고 생각을 정리해야 할 시점입니다. 펄스 자체는 매우 좋습니다. 
효율적으로 처리할 수 있어야 합니다. 추상적인 아이디어는 잘 만듭니다. 
구현이 따라온다면 많은 아이디어를 실현할 수 있습니다. 

## 개선 

신기한 경험입니다. 밤에 자고 오전까지 고민을 하면서 생각을 정리하는 중에 
std::pair를 키로 하여 여러 필드에 대한 검색이 가능한 점을 활용하면 
토픽과 세션에 대한 구독을 한번에 처리할 수 있다는 점을 알게 되었습니다. 

그리고 펄스 계층의 최상위 루트에 알려주고 전달 받아서 자체 구독을 처리하면 
되는 과정도 자연스럽게 나타났습니다. 

이와 같이 깊이 고민하면서 연습을 계속하면 구현하는 힘이 생깁니다. 항상 
노력할 일입니다. 노력보다는 깊이 빠지는 flow를 경험한다는 자세가 더 나을 
듯 합니다. 



