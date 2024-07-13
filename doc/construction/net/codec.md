# codec 

코덱은 code / decode로 바이트 변환을 합니다. 
통신에서 바이트 변환은 필수적이고 자주 나타납니다. 

암호화나 URL 인코딩 등 서로 다른 시스템 간에 바이트 배열을 
안전하게 전달하거나 서로 알 수 있는 형식으로 만들어야 하기 때문입니다. 

이와 같은 코덱을 프로토콜에 직접 구현하면 복잡도가 올라가고 
단위 테스트가 어려운 문제가 있습니다. 단위 테스트가 안 되면 
저처럼 버그를 잘 만드는 프로그래머는 안심할 수가 없습니다. 
그리고, 다른 기능들 사이에서 버그가 나오면 잡기 어렵기 때문입니다. 

또 성능 확인과 같이 통신에서 중요한 검증을 거치기도 어렵습니다. 
단위 성능을 미리 확인해 두면 큰 도움이 됩니다. 

## 길이 코덱 

length_delimited는 길이로 구분된이라는 뜻으로 길이 필드 4바이트를 추가하여 
프레임(패킷)을 구분하게 합니다. 

매우 단순한 기능인데 코덱으로 둔 이유는 프로토콜에서 여러 번 구현하지 않고 
재사용할 수 있도록 하기 위해서입니다. 

거의 대부분의 프로토콜을 길이로 구분된 프레임으로 만들어 전달할 수 있기 
때문입니다. 

## sodium 코덱 

소디엄은 소금입니다. libsodium은 안전한 공개키 기반의 키 생성과 교환을 
통해 게임에 딱 맞는 암호화를 구현할 수 있습니다. 인터페이스는 C를 따르지만 
사용법이 매우 단순합니다.

chacha20을 암호화에 주로 사용하는데 xor이 붙은 함수로 암복호화에 모두 
사용할 수 있고 inplace (동일 메모리를 사용한) 암복호화도 지원하고 
패딩이 필요 없어서 입력 길이와 동일한 출력을 제공합니다. 

대신에 키 교환 과정이 있어서 sodium_handshake라는 클래스 안에 
그 기능을 넣습니다. 이렇게 기능으로 분리하고 추상화가 가능하면 추상화를 
하고 단위 테스트로 해당 기능을 확인하는 흐름으로 구현합니다. 


## 단위 테스트 

### sodium 

단위 테스트는 목업(mockup)을 만드는 과정이 편리해야 합니다. 인터페이스를 
간결하게 하거나 람다 함수를 사용할 수 있도록 하는 방법으로 클래스 구현을 
하면 더 편리합니다. 

- streambuf 처리에서 consume이나 commit을 잊는 경우가 있다. 
- 단위 테스트에서 통신 처럼 처리하려니 함수를 분리해야 했다. 
  - send_key(), send_nonce()로 분리 
- 통신 디버깅은 hex로 dump해야 볼 수 있다. 
  - logger::dump_hex() 추가
- 송수신 키, nonce가 같은데 암호화가 깨진다. 

2. rx_key. dump: 73|ed|5e|82|8a|35|b9|cf|eb|68| e|24|a5|30| 6|cb|64|b8|a3|39| 4|9e|c4|f0|7e|80|6d|1a|72|93| 4|51|
1. tx_key. dump: 73|ed|5e|82|8a|35|b9|cf|eb|68| e|24|a5|30| 6|cb|64|b8|a3|39| 4|9e|c4|f0|7e|80|6d|1a|72|93| 4|51|

2. tx_key. dump: de|c5|7b|ea|1f|89| c|5b|80|51|8e|4d|10|b4|1a| 1|8f|59|e3| 8|11|67|61|92|74|b8| 7| f|bb|76|d8|7c|
1. rx_key. dump: de|c5|7b|ea|1f|89| c|5b|80|51|8e|4d|10|b4|1a| 1|8f|59|e3| 8|11|67|61|92|74|b8| 7| f|bb|76|d8|7c|

2. rx_nonce. dump: c3|51|b5|3f|96|5e|96|63|
1. tx_nonce. dump: c3|51|b5|3f|96|5e|96|63|

2. tx_nonce. dump: fb|f1|84|de|5c|a3|eb|a0|
1. rx_nonce. dump: fb|f1|84|de|5c|a3|eb|a0|

- 사소한 실수로 handshake를 같은 걸 써서 헤맸다. 그런 일은 많다. 
- \0를 문자열에 포함해서 보내고 std::string을 만든 오류도 있다. 

- 이런 과정을 밟으면서 asio::streambuf, sodium에 많이 익숙해졌다. 
- 단위 테스트와 디버깅을 위한 로그를 깔끔하게 만들고 추가 테스트를 한다. 
  - sodium_protocol을 잘 만들 수 있을 지 확인한다. 
  - sodium_protocol도 단위 테스트 가능하게 한다. 


- [x] cipher 반복 테스트 OK
- [ ] 로그 정리