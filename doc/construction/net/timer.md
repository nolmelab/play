# timer 

runner에서 실행하는 timer이다. chrono를 사용하여 인게임 이벤트 스케줄링 
같은 시간 상의 예약과 같은 기능도 수월하게 처리하면 좋다. 

once()가 필수적이므로 이를 구현하고 앞으로 나아간다. 

runner 문맥에서 관리하고 실행하도록 한다. 

- 타이머 참조와 취소 
- 타이머 오브젝트 풀 

## CompletionToken 넘기기 

```c++
  ts.once(std::chrono::milliseconds(300),
          [](boost::system::error_code& ec)
          {
          });
```

위 호출이 에러 난다. 

```c++
template <typename CompletionToken>
inline timer::ref timer_service::once(asio::chrono::milliseconds ms, CompletionToken&& handler)
{
  auto ref = std::make_shared<timer>(ioc_);
  ref->once(ms, std::forward<CompletionToken>(handler));
  return ref;
}
```
once 함수의 정의는 위와 같다. 

공포스러운 에러가 나와서 당황했는데 const boost::system::error_code&로 
아규먼트 타잎을 써야 한다. 

앱에서 이렇게 하기는 귀찮을 듯.

once는 괜찮은데 repeat가 문제. 람다로 재귀를 구현하기가 좀 더 어려워서 그렇다. 
몇 가지 기법들이 있다. 함수를 사용하는 것으로 했다. 더 이해하기가 쉽다. 

예약 함수와 반복 함수를 구분했다. 더 이해하기가 쉽다. 

## 풀 구현 

custom deleter를 갖고 construct() 호출로 inplace new를 하는 object pool을 만들어 
테스트 한다. 


