# object_pool 

custom deleter를 갖고 construct() 호출로 inplace new를 하는 object pool을 만들어 
테스트 합니다. 

## 조사 

https://github.com/massimo-marino/object-pool/blob/master/src/objectPool.h
- custom deleter를 사용하는 풀이다. 

그냥은 잘 안 보입니다. 위 아이디어와 코드에 기초하여 shared_ptr 기반으로 찾아 봅니다. 

## 구현 

deleter를 shared_ptr에 주는 방법을 검색하고 나서 금방 구현했습니다. 

아주 간단한 코드이고 어떤 오브젝트에도 사용할 수 있다는 점이 마음에 듭니다. 




