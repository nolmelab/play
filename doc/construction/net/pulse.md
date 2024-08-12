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

test_pulse.cpp를 작성하고 버그 수정을 했습니다. 테스트는 계속 추가해야 합니다. 

## call() 

subscribe() 흐름을 유지한 상태에서 call() 할 경우 요청을 보관하고 있습니다. 
단선에 대해 구독하고 해당 세션이 끊어지면 통지를 합니다. 

통지는 다른 함수로 이루어집니다. 이 흐름이 가장 단순합니다. 

분산 처리에서 에러 처리를 하려면 상태를 저장해야 합니다. 이를 call() 기능을 
통해 단선이 발생할 경우에 통지해서 에러 처리를 할 수 있다면 개별적으로 
기록하고 단선시 처리하는 코드를 작성하지 않아도 됩니다. 거의 모든 요청/응답에 
해당하기 때문에 매우 편리하게 분산 처리를 구현할 수 있습니다. 물론, 분산 
처리는 더 많은 과제들을 갖고 있지만 월드/게임 서버로 분리되는 구조에서 
그렇다는 뜻입니다. 

생각을 계속 했는데 요청/응답의 통신만 이와 같은 구조가 필요하기 때문에 
별도로 요청 아이디 관리가 필요 없다는 걸 알았습니다. tcp 기반이라 유실이 
없고 세션 연결이 맺어진 후에 요청 없이 응답이 없기 때문에 특정 세션의 
특정 토픽 요청 개수를 알면 다음 응답의 개수도 알 수 있기 때문에 이를 
아이디로 사용할 수 있습니다. 

<session, <topic, {request count, topic response count}>> 와 같은 구조를 갖고 
콜백 처리를 할 수 있습니다. 

subscribe()로 등록한 함수에서 정상적인 처리를 하고 call() 시 등록한 콜백에서 
에러 처리를 하는 것으로 해도 되는데 인터페이스는 구현하고 사용하면서 개선하는 것이 
가장 좋은 방법입니다. 

with_session()으로 특정 세션을 지정한 자식에 대해서만 call() 가능하도록 제한합니다. 
제한하면 더 안정적이 됩니다. 민감한 부분이 있으므로 그렇게 합니다. 

call() 관련 구현에서 동적인 구독과 프레임 처리를 post()를 통해 분리해야만 
락 이슈를 성능 저하 없이 해결할 수 있었습니다. off by one frame 패턴이라고도 
불릴 수 있겠습니다. asio의 가장 큰 장점입니다. 

사소한 실수가 꽤 많이 나오는데 clang과 vscode 툴에서 visual studio와 좀 다르게 
동작하면서 실수하는 지점이 늘었습니다. 익숙해지면 나아질 듯 합니다. 

아직 완벽하다고 할 수는 없지만 꽤 많이 좋아졌습니다. feature를 늘리지 않으면서 
현재 기능이 완전하도록 노력해야겠습니다. 

