# 베타 

## 기능 

- box2d, grid 기반의 mmorpg 
- animated 2d rigid body를 사용한 action rpg 
  - ground 2d shape와 height를 사용한 판정 
  - navimesh의 convex polygon화를 통한 판정 통합 
- navimesh (detour) 기반의 이동
- stat, ability, item, skill, behaviortree
- battle focused 
- viewer client (ensure viewer) 
  - projectiles move 
  - skill casting 
  - animation 
  - little effects
- element 
  - defines game data

알파의 rcs에 기초하여 인증, 월드, 프론트의 분산 시스템을 구현합니다. 프론트 서버는 
인증 서버와 자신의 월드 서버에 접속합니다. 

맵 인스턴스가 만들어지면 해당 맵 내에서 플레이를 합니다. 연결된 맵이나 포털로 이동합니다. 
맵 간 이동은 다리나 비행과 같은 심리스 구간을 사용할 수 있습니다. 이는 연출에 해당합니다. 

근간에서는 공간 분할된 인스턴스를 사용합니다. 심리스를 추구하지 않습니다. 심리스 처리는 
처리 모델이 pc나 npc와 같은 수준의 액터로 내려와야 하고, 그러면 공용 오브젝트에 대한 
락이 필요합니다. 그러한 모델은 처리 모델만 따로 큰 맵 상의 이동과 채팅으로 간략하게 
구현해 볼 수도 있겠습니다. 아마도 엡실론 이후가 될 예정입니다. 

