# 구조 

- service is the core 
  - lobby_runner
  - user_master
  - room_master
  - room_runner

- remote_call_service
  - RPC와 비슷하나 비동기로 동작한다. 
  - 타임아웃 지정이 가능하고 단선시에도 통지한다. 
  - 응답이 항상 보장되는 호출 서비스이다. 
  - 취소 기능은 지원하지 않는다. 

## service / system / actor / act

### logical structure 

actors act in service or in system. 
This is the main idea of application structure model of play. 

- act can be connected with signal2 slots. 
- persistent acts load or save data from db in db_runner and completed in its own strand. 
- bt acts are controlled by bt act (act_bt)

### processing structure 

server application reacts to messages and timers 
- timers can drive active actions 
- message are handled reactively 
- posting timers and messages are the key of processing 
  - either in multithread context or 
  - in dedicated strand 

let's call messages and timers events

chain of events are delivered to acts of actors. 
those are handled and responded. 

there are minor (important) details, but the above is the basics (fundamental).

## message naming convention 

메세지를 일관되게 이름 지으면 이해하기 편리합니다. 프로젝트별로 선호하는 방식을 취하면 됩니다. 
일관성이 가장 중요합니다. 

message name := [cluster::] namespace _ direction _ command 

direction prefix := req | ack | syn 

namespace := cluster | auth | room | ... 

command := verb | adjective (for states)

namespace는 flatbuffers의 namespace를 사용합니다. 앱에서는 전체 namespace를 사용합니다. 

topic으로 사용할 enum의 이름은 다음과 같이 결정합니다. 

- [cluster_]namespace_prefix_command
  - cluster_room_req_create 
  - auth_req_login 
  - auth_ack_login 

기능별로 나눠지고 명확해 보입니다. flatbuffers에서 namespace 처리를 하나의 파일에서 여러 개 
추가가 가능하고 잘 동작합니다.  

## 진행 

- robust
  - app 
    - object 
    - service

- alpha 
  - app

  - service
    - cluster::syn_service_up / cluster::syn_service_down 

  - auth
    - auth::req_login / auth::ack_login 
    - auth::req_logout / auth::ack_logout

  - room
    - room::req_create / room::ack_create
    - room::req_list / room::ack_list
    - room::req_join / room::ack_join

    - cluster::room::req_create
    - cluster::room::ack_create
    - cluster::room::syn_reserve



