# protocol 

protocol is application specific payload management layer. 

## related concepts (classes)

- codec 
    - encode / decode bytes to bytes    

- handshaker 
    - cipher_handshaker

- codecs
  - length_delimited
  - sodium_cipher  

- frame is a packet


## responsibility 

- handles session state notification frames 
- transforms frame into network packets and send with session 
- make frames from received bytes and dispatches them with frame_dispatcher 

## protocl examples 

- plain_protocol<Topic, Adatper> 
    - length_delimited 

- secure_protocol<Topic, Adapter>
    - length_delimited 
    - sodidum_cipher 
    - cipher_handshaker

- json_protocol<Topic, Adapter>
    - topic field
    - mark_delimited


Adapter는 협상 완료 통지와 바이트 전송, 프레임 바이트 수신을 처리하는 함수를 제공합니다. 
이를 통해 Protocol과 session 처리를 분리합니다. 이런 적응층이 없으면 session 내에 프로토콜 
코드가 혼재하게 됩니다. 

## adapter 구현 

std::function을 갖고 있도록 일단 했습니다. std::function은 함수 호출보다는 비용이 높습니다.  
따라서, 함수 포인터나 람다를 직접 갖도록 하는 것이 좋겠습니다.  
람다도 함수 포인터로 변환이 가능하므로 타잎을 맞춰서 지정할 수 있습니다. 

가상 함수도 함수 호출만큼 빠릅니다. 호출 비용은 거의 없습니다. 코드에 따라 변화는 있겠고 
결국 컴파일러의 최적화에 의존합니다. 

따라서, 일단 std::function으로 구조를 만들고 그 이후에 생각해 봅니다. 통신 쪽 처리에서는 
충분한 성능을 제공하기 때문입니다. 

### 템플릿 파라미터로 전달 

duck typing을 사용하고 template parameter로 전달하여 적합한 구조를 선택하도록 합니다. 

이와 같이 선택 가능한 옵션을 템플릿 파라미터로 다양하게 제공할 수 있다는 점이 C++의 
최대 강점입니다. boost나 stl 모두 템플릿인 점은 시사하는 바가 큽니다. 

session을 작성할 때 protocol에 전달하는 Adapter는 어떤 형태가 가장 좋을까요? 

```c++
struct session_protocol_adatper_1
{
  session_protocol_adatper_1(session& s) : s_(s) {}

  void send(...) { s_.send(...); }
  void forward(...) { s_.forward(...); }
  // 나머지 함수들도 유사하게 구현 가능합니다. 
};
```

기존의 std::function 기반 Adapter는 기본 어댑터로 남겨둡니다. 가장 유연하고 사용하기 편리하기 
때문에 쓰일 곳이 있을 듯 합니다. 

### session_handler와 함께 사용 

Protocol의 구현은 Adapter와 함께 session_handler를 사용합니다. 

협상과 변환을 처리하는 Protocol과 session 목록을 관리하는 개념과 잘 맞지 않기 때문에 
핸들러로 분리하는 것이 더 명확합니다. 

### 안 됨 

session::protocol_adapter가 session에 의존하고 다시 protocol은 adapter에 의존하므로 
상호 의존 문제가 발생했습니다. 

문제의 원인은 session에서만 send를 할 수 있어 어떻게든 session에 접근해야 하고 
protocol은 session의 템플릿 파라미터가 되어야 하기 때문입니다. 

결국 타잎을 지운 구조만 가능하고 virtual 클래스로 해야 하겠습니다. 



