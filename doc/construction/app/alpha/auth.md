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

### ensure 




## lobby_runner 

### auth::req_login 


### cluster::auth::req_login 


### cluster::auth::res_login


### cluster::auth::syn_login


### auth::res_login 




