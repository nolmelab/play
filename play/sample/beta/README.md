# beta

beta는 MORPG / MMORPG를 만들기 위한 기반 프로젝트입니다. 

알파의 분산 처리 기반 위에 공간 처리와 RPG 기능을 구축하여 
2d + height 기반의 판정이 가능한 게임을 만듭니다.

recastdetour, box2d의 라이브러리를 충실히 활용하고 
animated 2d rigid body를 사용하여 non-target rpg를 만듭니다. 

처리 구조는 알파와 동일하게 월드 / 게임 서버 구조를 갖습니다. 
알파의 코드를 그대로 복사한 후 alpha namespace를 beta로 변경하고
front / back을 front / world로 변경하고 auth 서버를 추가하여 
인증 후 월드 진입 과정을 구현합니다. 

이 작업이 완료되면 데디케이트 서버 방식이 아닌 대부분의 MMORPG, 
MORPG를 만들 수 있는 기초가 완료됩니다. 

beta를 만들면서 게임 개발에서 가장 중요한 게임 데이터 작업 프로세스를
함께 구축하고, 영속적인 속성을 저장하고 관리하기위한 RDBMS 처리를 
구현합니다. 

beta는 알파에 비하면 훨씬 큰 프로젝트입니다. 시간을 갖고 요소 각각에
충분한 시간을 들이고 learning을 위한 코드도 충분히 작성하여 
단단한 근간이 되도록 합니다. 



