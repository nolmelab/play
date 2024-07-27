# auth 

프로토콜 namespace로 구분한 기능 단위로 진행합니다. 

## alpha app 

알파는 secure_protocol<uint16_t, flatbuffers::NatvieTable> 을 사용하는 서버입니다. 
ensure처럼 thread_runner와 server를 갖습니다. flatbuffer_handler를 두고 각 서비스에서 
필요한 topic에 sub합니다. 

### app의 시작 

- lobby_runner 서비스 골격 작성 
- alpha.hpp와 alpha.cpp 구현 
- 서비스 생성과 시작 
- 서비스 종료
- 앱 종료 

- namespace 전체 리팩터링
  - play로 단일화 
  - play::ensure는 ensure로 변경 
  - std:: boost::asio:: 수준이 적절하다 
  - boost만큼 큰 기능 분할이 없으므로 play, ensure, alpha / beta 정도가 적절


### ensure

- ensure_alpha가 ensure_app을 상속. 내용은 거의 없이 재사용 
- ensure/act_base를 추가하여 app과 bot에 접근 가능하도록 함 
  - alpha 액트들을 기반 클래스 
  - alpha에 act_base_alpha 클래스 추가 
    - 나중에 보면 유틸리티 기능들이 필요하므로 공용 기반 클래스로 사용 

## 프로토콜 규약 

- f2b, b2f를 postfix로 추가하여 서버간 통신 명시 
  - 서버 역할이 많아져도 비슷한 두자 약어로 가능 
  - 짧고 명확해서 괜찮음 
  - f2w (front to world), f2a (front to auth)와 같은 형태
  - 분산 처리를 하면 처리해야 하는 프로토콜 수가 2배 가까이 증가. 
    - 특히, login, join 처럼 컨테이너에 대한 처리 프로토콜이 그렇다. 

## alpha 앱 






