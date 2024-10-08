# db

## 목차 

- [nanodbc](./nanodbc.md)
- [db](./db.md)
- [db 처리 사슬](./db_process_chain.md)

db는 게임에서 상태 값의 저장에 사용합니다. 게임의 상태 또는 데이터는 
세 가지로 나눌 수 있습니다.

- 정적인 상태 / 데이터 
  - 기획에서 설정한 값들 
  - 기획 데이터 
- 동적인 상태 / 데이터 
  - 게임의 실행 중 생기는 변환 
- 저장 상태 
  - 어딘가에 저장하고 다시 읽기 가능한 상태
  - db (rbms, nosql db 등) 저장과 읽기 

여기서 저장 상태를 처리하는 부분 중 핵심이 db입니다. 예전에는 파일에
저장하기도 했고, 아직 싱글 게임에서 쓰이고 있습니다.


## db의 선택

db는 nosql 나온 이후 매우 많은 선택지가 있습니다. 전통적으로 게임 서버는
rdbms를 사용했습니다. mysql이나 oracle, sql server 등을 주로 봤습니다.
안정성이 높고 read uncommitted와 같은 격리 수준(isolation level)을 
사용할 수 있는 게임에서는 성능도 비교적 괜찮기 때문입니다. 

또 가끔은 트랜잭션이 필요한 경우가 있는데 지원하지 않는 db로는 처리의 
정확성을 보장하기가 어려울 때가 있습니다. 

저는 이번에 postgresql을 사용해 보려고 합니다. 성능 면에서도 괜찮고 
오래 되어 안정적이고 버전 기반이라 동시성 지원도 좋다고 합니다. 
mysql이나 sql server 등을 별로 사용해보지 않은 측면도 있지만 
뭔가 더 멋져 보이기 때문입니다. pgAdmin이 GUI가 Hedidsql보다 더 
예뻐서 그런 듯도 합니다.

## api

db에서 제공하는 api를 선호하는 개발자들도 있습니다. 저는 odbc로 
충분하다고 생각하는 편입니다. 게임 서버에서는 더 많은 지원이 필요 없기도 
하고, db 처리는 쉽고 편하고 빠르게 작성할 수 있으면 된다고 생각하기
때문입니다. 물론 근간은 정확하고 안정적으로 동작해야 합니다.

c++에서 odbc api 라이브러리는 검색되는 대상 중 nanodbc가 그나마 
제일 나은 듯 합니다. 직접 하나 만드는 것도 생각해 볼 수 있으나 
nanodbc로 충분한 듯 합니다. nanodbc도 불편한 면들이 있으나 
약각만 익숙해지면 괜찮은 듯 합니다. 



