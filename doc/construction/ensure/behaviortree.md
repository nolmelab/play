# behaviortree

behaviortree.cpp를 소스로 external에 포함하여 빌드하고 
사용합니다. 포크된 프로젝트로 하고 필요한 기능 수정, 버그 수정을 하면서
해당 프로젝트에 기여하도록 합니다. 어려우면 playact로 프로젝트를 
변경하여 직접 수정하도록 합니다. 

## behaviortree.cpp fork 

playact로 fork 합니다. 
external에 둡니다. 
git의 submodule로 관리합니다. 


### git의 submodule 

git submodule add로 가능하다.

다른 pc에서 사용할 경우, git submodule remote --init path_to_the_submodule 로 초기화 필요. 
이후 update를 하면 된다. 여기서 수정이나 개선을 해도 된다. nanodbc는 그렇게 하자. 


### behaviortree.cpp cmake

zeromq를 필요로 합니다. 이런 면들이 좀 귀찮을 수도 있고, 플래폼 호환을 유지하기 어려울 수도 있습니다. 

- zeromq를 vcpkg에 설치하고 해결하기
    - 바로 해결. vcpkg 좋다. cmake의 힘이다. 결국은 사람의 힘

- gtest (google test) 추가 설치 

이후 빌드 된다. 


### unreal cmake support

unreal 빌드 툴에서 cmake 모듈을 지원할 수 있는 방법을 잠시 찾아 봅니다. 

https://github.com/caseymcc/UE4CMake

이런 노력들이 있기도 해서 어느 정도 가능할지도 모르겠다는 생각을 해봅니다. 

https://github.com/aws/aws-sdk-cpp/issues/2369

여기 답 중 하나가 인상적이다. 결국은 필요한 라이브러리를 플래폼 별로 설치하고 
빌드하는 것이다. 안드로이드 빌드는 또 다른 문제이겠으나 근간은 같다. 
편리한 정도만 다르다.

 







