# find vcpkg packages_
find_package(spdlog CONFIG REQUIRED)
find_package(doctest CONFIG REQUIRED)
find_package(boost_asio CONFIG REQUIRED)
find_package(unofficial-sodium CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

set(SOURCE 
  main.cpp
  app.cpp
  front/lobby_runner.cpp
  front/lobby_user.cpp
  front/room_runner.cpp 
  front/room.cpp 
  back/lobby_master.cpp 
  back/room_master.cpp 
  back/master_room.cpp 
  back/master_lobby_user.cpp 
)

set(INC
  "${CMAKE_SOURCE_DIR}/.."
  "${CMAKE_SOURCE_DIR}/sample/"
)

set(LIBS 
  play_base 
  play_net
  play_app
  spdlog::spdlog 
  doctest::doctest
  unofficial-sodium::sodium
  nlohmann_json::nlohmann_json
  pthread
)

# add test target
add_executable(
  alpha
  ${SOURCE}
)

if (MSVC)
  target_compile_options(alpha PRIVATE /MTd /utf-8)
else() 
  target_compile_options(alpha PRIVATE -g -fstack-protector-all -Wno-unused-value)
endif()

# add link libraries. playground_core is available with cmake
target_link_libraries(
  alpha
  ${LIBS}
)

target_include_directories(
  alpha 
  PRIVATE 
  ${INC}
)
