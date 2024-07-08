# setup vcpkg 

- home pc
  - windows 
  - vc2022
  - vcpkg
    - boost
    - doctest
    - spdlog

## install

- vcpkg already installed (c:\app\vcpkg)
- installing boost takes time 
- doctest already installed

### spdlog 

```
find_package(spdlog CONFIG REQUIRED)
target_link_libraries(main PRIVATE spdlog::spdlog spdlog::spdlog_header_only)
```

settings.json in .vscode can have VCPKG_TARGET_TRIPLET.

