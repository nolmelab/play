# stream_protocol 구현 

[1] excpetion 처리 추가 : server와 client 모두 필요한 곳에 추가
[2] connect / accept 후 통신이 진행되지 않음
- established가 호출되나 send()가 동작하지 않음
- stream_protocol::accepted()에서 호출 하지 않는 버그 OK
- 이후에도 통신이 안 됨
- socket_.async_receive()로 해야 일부만 받음. asio::async_read()는 버퍼 크기만큼 받음
- 이제 잘 됨
[3] native_handle 소켓에서 얻은 값이 이상하여 start()에서 지정하도록 변경
- 제대로 보임
[4] reuse address 옵션 추가. 디버깅할 때 매우 불편
- 서비스 할 때는 TIME_WAIT 값 조절로 해결해야 함
- 포트를 훔칠 수 있으므로 옵션으로 하는 것 고려
[5] 통신 되고 나서 poll()에서 리턴되지 않고 계속 실행됨
- accept가 요청된 상태에서 run()을 호출하면 대기하게 됨
- poll_one()을 호출하는 함수를 추가
[6] 전체 구조 변경 후 릴리스 모드에서 연결만 되고 통신이 안 됨
- for 루프에서 i를 초기화 하지 않아서 생긴 문제

echo는 성능 측정용이 아니긴 하나 기본 특성을 살핀다.
send의 시작 크기가 늘어나면 초당 송수신 바이트는 늘어난다.


    // 릴리스 빌드:
    // [1] 대역 사용 (bandwidth throughput)
    // - 페이로드 시작을 500 바이트로 늘리면 초당 100메가 바이트를 송수신 한다.
    // - 에코이므로 실제 성능은 더 늘어난다.
    // [2] 패킷 사용 (pps throughput)
    // - 1바이트로 PPS를 측정하면 6초에 10만개 정도이다.
    // - 에코이므로 마찬가지로 제한되지만 괜찮다.