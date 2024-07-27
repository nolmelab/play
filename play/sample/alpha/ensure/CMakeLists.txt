project("ensure_alpha")

# find vcpkg packages_
find_package(spdlog CONFIG REQUIRED)
find_package(doctest CONFIG REQUIRED)
find_package(boost_asio CONFIG REQUIRED)
find_package(unofficial-sodium CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# add test target
add_executable(
  ensure_alpha
  ensure_alpha.cpp
  main.cpp
)

if (MSVC)
  target_compile_options(ensure_alpha PRIVATE /MTd /utf-8)
else() 
  target_compile_options(ensure_alpha PRIVATE -g -fstack-protector-all -Wno-unused-value)
endif()

# add link libraries. playground_core is available with cmake
target_link_libraries(
  ensure_alpha
  play_base 
  play_net
  play_app
  play_ensure
  spdlog::spdlog 
  doctest::doctest
  unofficial-sodium::sodium
  nlohmann_json::nlohmann_json
  pthread
)

# add include directories, especially playground
target_include_directories(
  ensure_alpha 
  PRIVATE 
  "${CMAKE_SOURCE_DIR}/.."
  "${CMAKE_SOURCE_DIR}/sample/"
)

