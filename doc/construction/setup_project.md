# setup project 

home and codespace 

## home 

- windows 11 
- vs2022 
  - cmake project 
- vcpkg

### vs2022 upgrade

- 17.10.3 

## codespace

- ubuntu 
- vscode
- clang 
- cmake project 
- vcpkg

## directory structure 

- doc 
- play
  - learn
  - robust 
    - base 
    - element
    - net 
    - server
      - base 
        - logger.hpp
        - object.hpp
        - service.hpp
        - system.hpp
        - actor.hpp
        - controller.hpp
        - property.hpp
      - db
        - db_service
      - auth
        - auth_server
        - auth_be_service 
        - auth_fe_service
      - world 
        - world_server
        - instance_service
          - instance
      - game
        - game_server
        - instance_service
          - instance
          - actor 
            - character.hpp
            - pc.hpp
            - npc.hpp
          - controller 
            - inventory_controller.hpp
            - behavior_controller.hpp
            - skill_controller.hpp
            - buff_debuff_controller.hpp 
            - quest_controller.hpp



## vs2022 

- cmake project support 
  - c++ cmake tools needs to be installed
  - open local folder where CMakeLists.txt exists
  - vs2022에서 cmake 프로젝트를 잘 지원한다. 
    - CMakeSettings.json 파일로 구성한다. 
      - vcpkg 옵션 지정도 수월하다. 
      - CMakeSettings 전용 GUI를 갖고 있다. 
    
아직 익숙하지 않기도 하겠으나 intellisense가 잘 동작하지 않는다. 
vscode에서 빌드 에러 이동이 잘 안 되기 때문에 딜레마이다. 





