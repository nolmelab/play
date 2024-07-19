# flatbuffers 

## 컴파일러 

sudo apt install -y flatbuffers-compiler

매우 편한 패키지 세상입니다.

flatc 명령이 동작합니다. 

## 라이브러리 

vcpkg에 포함되어 있습니다. 해당 버전을 사용하고 나중에 언리얼 모듈을 
만들 때는 소스를 포함하도록 합니다. 

언리얼의 robust 모듈은 playrobust에서 소스를 릴리스 하는 방식의 프로젝트가 될 예정입니다. 

언리얼은 여러 플래폼을 지원하기 때문에 모듈도 여러 플래폼을 지원해야 하고 
vcpkg 의존을 갖고 안드로이드나 ios 빌드를 하기 어렵습니다. 물론 윈도우 빌드도 안 됩니다. 

find_package(flatbuffers CONFIG REQUIRED)
target_link_libraries(main PRIVATE flatbuffers::flatbuffers)



