# client와 server 

- client 
  - has a session
  - start connecting 
  - reconnect on disconnect 

- server 
  - has sessions 
  - listen 
  - accept 
  - handle disconnect 

## 구현 

- thread_runner를 포함합니다. 
- template 클래스로 Protocol을 받습니다. 
  - server<plain_protocol<uint32_t>> 로 테스트 합니다. 
- listen을 확인합니다. 

- client를 만들어야 통신이 가능하므로 같이 구현합니다. 
  - 연결을 시작합니다. 
  - 연결 실패시 재연결을 시도합니다. 
  - 타이머로 연결 시간을 제어 합니다. 

- 단위 테스트를 echo까지 충분히 잘 할 수 있어야 합니다. 
  - stream_protocol 추가 
  - 간단한 bypass 프로토콜 

