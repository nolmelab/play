# flatbuffers 

## 컴파일러 

sudo apt install -y flatbuffers-compiler

매우 편한 패키지 세상입니다.

flatc 명령이 동작합니다. 

## 라이브러리 

vcpkg에 포함되어 있습니다. 해당 버전을 사용하고 나중에 언리얼 모듈을 
만들 때는 소스를 포함하도록 합니다. 

언리얼의 play 모듈은 playplay에서 소스를 릴리스 하는 방식의 프로젝트가 될 예정입니다. 

언리얼은 여러 플래폼을 지원하기 때문에 모듈도 여러 플래폼을 지원해야 하고 
vcpkg 의존을 갖고 안드로이드나 ios 빌드를 하기 어렵습니다. 물론 윈도우 빌드도 안 됩니다. 

find_package(flatbuffers CONFIG REQUIRED)
target_link_libraries(main PRIVATE flatbuffers::flatbuffers)


## 구현 

TDD는 learn과 함께 잘 동작합니다. 생소한 기능을 learn의 테스트들로 이해를 깊게 하면서 설계 
아이디어를 얻습니다.  이렇게 얻어진 아이디어를 기존의 코드에 붙이면서 기능을 더 확인합니다. 

어느 정도 하다 보면 "아하"의 순간이 오고 설계가 완성됩니다.  이 과정에서 만들어진 코드가 
단위 테스트로 발전합니다. 그리고, 의도적인 커버리지로 추가 테스트를 만듭니다. 이 부분이 아직 어렵고 
나머지는 자연스러운 과정입니다. 마치 숨을 쉬듯이.

### 최종적인 모습 

- server<Protocol>
  - server<secure_protocol<uint16_t>>
  - server(..., frame_handler& handler)
  - server.get_handler<Handler>().reg(1, PLAY_FB_CREATOR(type));
  - server.get_handler<Handler>().sub(1, handler);
  - server.get_handler<Handler>().send(1, fb, true);

클라이언트도 같은 모습이겠지요.

```c++
using server = server<secure_protocol<uint16_t>;

flatbuffer_handler<server::session> handler;
server s(..., handler);

handler.reg(topic::req_login, ...);
handler.sub(topic::req_login, ...);

```

타잎에 대한 재귀 의존이 있어 server에 템플릿 파라미터 타잎으로 flatbuffer_handler를 제공하기 
어렵습니다. recv() virtual 함수는 final로 override하여 최적화 할 수 있으므로 (devirtualization)
괜찮아 보입니다. 

### frame_handler의 구현 

recv() 인터페이스와 타잎 정의들만 있는 순수 인터페이스 함수입니다. 

### flatbuffer_handler의 구현 

- reg() 
- sub() 

sub는 필요한 곳으로 넘기는 방식으로 합니다. 세션에 대한 구독이 아니기 때문에 앱 수준에서 
전달을 결정할 필요가 있습니다. 그리고, 앱과 수명을 같이 하는 핸들러가 적절한 시작점입니다. 




