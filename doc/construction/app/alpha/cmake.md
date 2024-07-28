# cmake 

cmake와 vscode 모두 중요한 도구입니다. vscode도 매우 좋아져서 완전 쓸만하게 되었습니다. 
특히, Linux에서 c++ 코딩이 원활하고 clang-format 등 도구들이 좋습니다. valgrind와 같은 
리눅스 개발 도구와 clang 등 visual c++과 다른 컴파일러의 빌드 결과를 함께 보면 
개발의 다른 측면을 보게 됩니다. 

msbuild도 customization이 가능한 도구이지만 대부분 visual studio에서 제공하는 기능만 
대체로 사용합니다. msbuild의 명령과 동작을 이해하는데에도 cmake에 대한 이해는 도움이 
됩니다. 

## cmake custom 명령

알파는 서버를 여러 개 실행해야 하고 test json 파일들도 갖고 있어
커스텀 명령으로 파일을 복사하여 구성할 필요가 있습니다. 

touch로 추가하여 간단하게 테스트를 했는데 커스텀 명령 자체는 잘 동작합니다. 

하고자 하는 작업은 alpha/bin 폴더를 alpha/app 폴더 하위로 복사하여 
서버와 테스트를 실행할 준비를 하는 것입니다. 

```c++
add_custom_target(bin ALL)

add_custom_command(
        TARGET bin POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_SOURCE_DIR}/*.json
                ${CMAKE_CURRENT_BINARY_DIR}/
)
```

위 명령어로 파일은 복사를 했습니다. 마지막 /를 붙여야 디렉토리로 인식합니다. 
이제 폴더 폴사를 해야 합니다. 

