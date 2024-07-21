# function_call 

https://github.com/skarupke/std_function/blob/master/function.h

std::function은 람다를 지원하기 때문에 virtual 보다 더 강력한 인터페이스 기능을 
제공합니다. virtual 보다 약간 더 빠른 성능을 보이기 때문에 더 빠르게 하고자 하는 
노력들이 있습니다. 

하지만, 성공적으로 보이지는 않습니다. 약간의 최적화가 조금씩 있기능 하지만 비약적으로 
빠르지는 않습니다. inline에 대한 방어벽이 되기 때문에 c++ 최적화의 핵심을 사용하기 
어려운 것이 가장 큰 이유입니다.

모든 컴파일러 라이브러리의 std::function 구현에서 최적화가 포함된다는 점이 중요합니다. 

아마도 작은 크기의 최적화는 모든 곳에 포함될 가능성이 높습니다. 또 점점 나아지고 있고 
c++ 표준에서도 개선하려는 노력이 있습니다. 

지켜보면서 고민할 예정입니다. 
