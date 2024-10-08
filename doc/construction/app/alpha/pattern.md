# pattern 

java로 디자인 패턴을 보고 프로그래밍을 할만하다 느낀 이후 패턴은 항상 화두 중 
하나였습니다. 테스트와 증명 가능할 정도의 정확성과 함께 프로그래밍을 잘 하기위한 
중요한 연습이자 도구라고 생각합니다. 

알파에서 반복되는 패턴이 있습니다. 통신과 처리, 처리를 이어가기, 단일 쓰레드와 
다중 쓰레드 처리는 pulse로 추상화 했습니다. 아직 개선해야 할 점이 많이 있지만 
시작은 좋은 것 같습니다. 

프론트와 백엔드 서비스와 중요 액터들을 구현하면서 일정하게 반복되는 패턴이 있습니다. 
추상화까지 진행은 하지 않을 듯 하나 한번 생각을 정리하고 반복하는 것도 좋을 듯 합니다. 

## 프론트 서비스와 액터 패턴 

프론트는 사용자 연결을 받는 펄스와 뒷단(들)과 연결하는 펄스 두 가지를 갖습니다. 
또 사용자나 액터를 담는 컨테이너가 필요합니다. 컨테이너는 키를 여러 가지 갖습니다. 
이는 actor_container로 아직 완전하지는 않으나 재사용할 수 있도록 했습니다. 

프론트 펄스에 세션과 토픽으로 구독하여 처리를 시작합니다. 액터를 만들 수 있으면 
만들고 프론트 펄스의 처리는 액터 내 펄스로 처리를 위임합니다. 

백 펄스는 요청 후 단선 에러 처리가 필요한 경우 call을 하고 통지만 필요한 경우는 
send로 처리합니다. 

액터 내에서 처리 중에 스스로 소멸해야 하는 경우 나를 포함하는 컨테이너에 
post()로 한 처리 흐름 이후에 처리되도록 하여 삭제를 요청합니다. 

이 흐름으로 대부분의 프론트 기능 구현이 가능합니다. 아마도 게임도 마찬가지
일 것으로 예상되나 beta 진행 시 더 보도록 하겠습니다. 

## 백엔트 서비스와 액터 패턴 

백엔드의 프론트 펄스는 여러 액터들이 공유하는 세션입니다. 따라서, 세션 만으로 
구분이 안 되는 특징이 있습니다. 백엔드의 서비스들과 프레임의 프로토콜은 
이를 구분할 필드를 필요로 합니다. 주로 name이나 uuid 또는 액터 아이디를 
사용하여 처리할 수 있습니다. 

백엔드 서비스에서는 컨테이너에서 찾아서 액터를 호출하는 방식으로 구현하면 됩니다. 
외부에서 프레임 처리를 요청하는 함수들은 do_로 시작하도록 했습니다. 
자체적인 프레임 구독과 처리는 on_으로 시작하므로 구분이 됩니다. 

백엔드에 연결된 여러 프론트 연결을 담고 멀티캐스팅을 할 필요가 있어 
session_container를 만들었습니다. 여기에 send() 함수를 추가하여 
전체 연결된 프론트 세션에 보내도록 했습니다. 

## pulse의 개선 

명확하지 않았는데 인증 과정을 백엔드까지 구현하고 나니 잘 보입니다. 

현재 interests는 토픽과 세션으로 분리하여 관리할 필요가 있습니다. 
둘을 합친 키는 많아지면 수 백만에 이를 수 있습니다. 테스트에서는 
느리지 않았으나 효율은 떨어집니다. 당장은 문제가 되지 않고 
인터페이스는 분리되어 있으므로 부하 테스트를 진행할 때 개선하도록 합니다. 


