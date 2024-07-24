# app

## robust/app 관계 

robust/app에 구체적인 역할을 갖는 서비스와 서버를 포함하는 것이 적절한지 아직 
판단하기 어렵습니다. 알파에서 근간을 구현하고 복제하여 베타에서 MO를 만들고 
sigma에서 MORPG/MMORPG를 만드는 것이 적합해 보입니다. 

robust는 기반이 되는 변경이 적은 부분에 대해 호환성을 유지하면서 개발을 하고 
app에서 복사 후 변경을 통해 다양한 게임 모델과 게임 기능을 만드는 체계가 
더 좋아 보입니다. 

## 프로젝트들 

- alpha : 분산 구조 / 인스턴스 채팅 / 월드 채팅 / sure
- beta : alpha, 공간 처리 / 액터 / 컴포넌트 / 행동 트리
- sigma : alpha, beta, 능력치 / 아이템 / 스킬 / 버프 / 디버프 등

## 진행 

alpha를 진행하면서 적절한 재사용 가능하고 추상화된 코드를 robust/ (주로 app)에 추가합니다. 

beta를 진행하면서 alpha를 개선하고, 적절한 기능을 robust에 추가하고 개선합니다. 

sigma가 완성되면 프로젝트도 완성됩니다. 이후는 다듬는 과정입니다. 
