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

### formatting 

linux-clang-x64 형식으로 지정 
google 포맷으로 지정 

### squiggle again

intellisense가 codespace에서 동작하지 않는다. 

c_cpp_properties 파일에 c++ 표준이 c++17이라 c++20 기능인 optional이 
지원되지 않았다. 

include 폴더를 지정하니 잘 된다. linux-clang-x64 intellisense는 잘 동작한다.  

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

- 방안: 
  - 리눅스에서 개발하고 윈도우에서는 빌드와 디버깅, 간단한 수정을 한다. 
  - vscode에서 개발하고 visual studio에서 빌드 오류 수정과 디버깅을 한다. 
  - visual studio의 cmake 환경이 제대로 동작하게 한다. 

### vs2022 cmake 설정 

- instellisense가 잘 동작하지 않는 문제 
CMakeSettings.json에    
 "intelliSenseMode" : "windows-msvc-x64",
추가. 

동작할까? 일단 CMake 구성이 되어야 CMakeCache가 만들어지고 
CMakeCache가 있어야 intellisense가 동작한다. 

intelliSenseMode를 windows-msvc-x64로 지정하고 동작한다. 

E. #include <boost/asio.hpp> 참조 찾기가 안 된다. 
> out 폴더를 지우고 cache를 다시 생성하니 된다. 
> cmake는 캐시에 매우 많이 의존한다. 잘 써야 한다. 

### vcpkg 설치 이슈 

기존에 설치된 vcpkg가 제대로 설치되지 않은 것을 알고 재설치를 진행
boost의 사용은 개별 라이브러리별로 find_packet()를 하도록 되어 있다. 

find_package(boost_asio CONFIG REQUIRED)
위와 같이 찾아서 사용한다. 

### MTd overring MDd option waring 

### asio WINNT definition warning 

### .ipp file intellisense not working


## vscode formatting 

- C_Cpp: Formatting : vcFormat 
- indent 옵션들 지정하면 됨 
- SHIFT + ALT + F 로 포맷









