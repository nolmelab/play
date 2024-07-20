# nanodbc

일부 개선할 필요가 있어서 직접 프로젝트에 포함했습니다. 포크를 하고 진행하는 방법도 
있겠으나 빠른 개선이 필요하고 중요한 부분이고 크기도 작아 내재화 해도 문제가 
없을 듯 합니다. 대신, 이슈가 발생하고 수정하면 nanodbc 프로젝트에 리포트 하도록 합니다. 

## dependency 

linux에서 unixODBC나 iODBC가 필요합니다. 2023년까지 유지 보수를 지속하고 있는 
프로젝트들인데 어느 걸 써야 할까요?  nanodbc에서는 둘 다 언급하고 있습니다. 

iODBC가 BSD 라이선스를 포함하고 있어 보다 자유로운 사용이 가능합니다. GPL을 
개인적으로 선호하지 않는 취향도 있습니다. 

하지만 vcpkg에 unixodbc만 포함되어 있습니다. 지원이 unixodbc가 낫다는 얘기가 
사실 같습니다. 

## unixodbc vcpkg 

find_package(unixodbc REQUIRED)
target_link_libraries(main PRIVATE UNIX::odbc)

## 빌드 

db 하위 프로젝트 빌드가 잘 진행되었습니다. db를 사용하기 편리한 기능을 테스트 위주로 
잘 작성하고 편리하도록 만듭니다. 








