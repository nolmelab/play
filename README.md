# play

MMORPG와 MORPG를 만들기 위한 게임 서버 기반(foundation)입니다. 

기반은 라이브러리나 프레임워크가 아니고 코드를 직접 수정하여 
원하는 게임을 만들 수 있도록 합니다. 퀘이크와 같은 초기 게임 엔진들을 
라이센싱 하여 사용했던 방식입니다. 

플레이는 MIT 라이선스를 갖습니다. 따라서, 제한 없이 사용할 수 있습니다. 

게임 개발자들이 플레이를 기반으로 멀티플레이 게임들을 많이 만들 수 있길 기원합니다. 

## 기능 

- 통신과 분산 처리 
- DB 처리 
- 게임 데이터 
- 게임 기능 

### 통신과 분산 처리 

pulse로 추상화하여 단일 인터페이스로 서버 실행, 클라이언트 실행, 구독과 처리, 
원격 호출의 단선 에러 처리를 일관되게 사용할 수 있도록 합니다. 

pulse 내에서 asio strand를 통한 처리, session 단위 구독 필터링을 포함하고 
타이머를 사용할 수 있게 합니다. 

이와 같은 추상화를 통해 서버 실행, 분산 처리, 클라 처리를 쉽고 편리하게 할 수 
있도록 합니다. 

```c++
  using pulse_t = pulse_fb<secure_protocol<uint16_t>>;

  size_t recv_count = 0;

  poll_runner runner{"pulse runner"};
  pulse_t pulse_server;
  pulse_t pulse_client;

  pulse_t::add_unpacker(topic_message, &pulse_t::unpack_fn<fb::req_move, fb::req_moveT>);

  pulse_server.as_server(&runner, 7000)
      .sub(topic_message,
           [](pulse_t::session_ptr se, pulse_t::frame_ptr fr)
           {
             auto req_move = std::static_pointer_cast<fb::req_moveT>(fr);
             auto v = req_move->pos->x();
             pulse_t::send<fb::req_move>(se, topic_message, *req_move.get(), false);
           });

  pulse_client.as_client(&runner, "127.0.0.1:7000")
      .sub(pulse_t::topic_estalished,
           [](pulse_t::session_ptr se, pulse_t::frame_ptr fr)
           {
             fb::req_moveT move;
             move.pos = std::make_unique<fb::vec3>(1, 1, 1);
             move.name = "hello";

             pulse_t::send<fb::req_move>(se, topic_message, move, false);
           })
      .sub(topic_message,
           [&recv_count](pulse_t::session_ptr se, pulse_t::frame_ptr fr)
           {
             recv_count++;
             auto req_move = std::static_pointer_cast<fb::req_moveT>(fr);
             auto v = req_move->pos->x();
             pulse_t::send<fb::req_move>(se, topic_message, *req_move.get(), true);
           });

  CHECK(pulse_server.start());
  CHECK(pulse_client.start());

  runner.poll_one();

  bool end = false;

  while (recv_count < test_config::test_frame_count)
  {
    if (runner.poll_one() == 0)
    {
      runner.sleep(1);
    }
  }

  pulse_server.stop();
  pulse_client.stop();
```

위와 같이 서버 / 클라 펄스를 만들고 서로 연결을 맺고 프레임 송수신을 설정하는 코드가 
매우 간결하고 단순합니다. 

libsodium 기반의 암호화 키 교환과 암호화도 내장되어 있습니다. 게임 개발에 필요한 
통신, asio 기반의 처리를 손쉽게 할 수 있습니다. 

## 예제 애플리케이션 (sample)

- 알파
  - 월드와 게임의 분산 처리
  - 게임 인스턴스의 분산과 관리
  - 서비스, 액터, 컴포넌트 구조
  - 위 기능의 실증 모델로서 채팅 앱
  - 기능과 부하 테스트 도구와 테스트 예시
 
- 베타
  - 캐릭터, 몬스터, NPC, 스폰 볼륨, 데미지 볼륨 등의 액터
  - 속성, 능력치, 아이템, 인벤
  - 스킬, 버프, 디버프
  - 공간 상의 이동과 전투
  - 행동 트리
  - RPG 기능의 실증 모델
 
