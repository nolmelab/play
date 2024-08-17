# 방 처리 

방 생성, 조회, 예약, 진입, 채팅, 진출로 이루어집니다. 

로비의 인증 흐름과 다르게 서버 간 토픽만 다르게 하고 프레임은 재사용하여 
처리를 구현해 보려 합니다. 

room_req_create_f2b, room_req_create_b2f 가 같은 플랫버퍼 테이블을 사용합니다. 

중복되거나 의미가 모호한 경우가 생기기는 하지만 약간만 주의하면 
비슷한 테이블을 반복해서 만들지 않아도 됩니다. 토픽만 분리하면 
구독과 처리를 분리할 수 있습니다. 

## 인증 확인 

이미 인증된 사용자인지 권한을 확인하도록 합니다. 두 서비스 간의 상호 
연동 구조를 확보하려고 합니다. 

## 개선 - flatbuffers 전송 

```c++
  room_.get_pulse_back()->send<room::res_leave>(topic::room_res_leave, res);
```

room::res_leve, topic::room_res_leave, res 세 가지는 모두 같은 정보를 공유하고 있습니다. 
flatbuffers의 Pack 방식 때문에 object api를 쓸 경우 필요합니다. 그렇지 않으면 
더 복잡한 템플릿 코드를 사용해야 합니다. 

이를 좀 더 단순하게 만들 방법은 없을까요? 
send(res); 가 가장 이상적입니다. 

PULSE_SEND(room_.get_pulse_back(), room, res_leave, res); 이 정도가 필요한데 이것도 
좋아 보이지 않습니다. 

현재 모두 타잎과 연결되어 있는 점은 괜찮습니다. intellisense와 컴파일러에서 모두 
잘 체크됩니다. 

## 프로토콜 정의 

세부 항목을 포함하여 정의할 필요가 있습니다. XMPP의 RFC 문서나 다른 IETF의 표준 문서들이 
분산 처리 표준을 어떻게 만드는지 공부하는데 도움이 많이 됩니다. 

### reserve / checkin / leave 

reserve는 room_master에서 알 수 있습니다. checkin 처리까지 할 필요는 없습니다. 
room_actor에서 일정 시간 오지 않는 reserve에 대해서는 leave 처리를 합니다. 

현실에서 예약과 노쇼(no showup)에 해당하는 프로토콜입니다. 

### room_info 

room_info는 ec가 fail이 아니면 항상 모든 정보를 갖습니다. 




