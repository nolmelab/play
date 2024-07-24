project("test_base")

# find vcpkg packages
find_package(spdlog CONFIG REQUIRED)
find_package(doctest CONFIG REQUIRED)
find_package(boost_asio CONFIG REQUIRED)
find_package(unofficial-sodium CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# add test target
add_executable(
  perf 
  main.cpp 
)

if (MSVC)
  target_compile_options(perf PRIVATE /MTd /utf-8)
else() 
  target_compile_options(perf PRIVATE -g -fstack-protector-all)
endif()

# add link libraries. playground_core is available with cmake
target_link_libraries(
  perf 
  play_robust_base 
  play_robust_net
  spdlog::spdlog 
  doctest::doctest
  unofficial-sodium::sodium
  nlohmann_json::nlohmann_json
)

# add include directories, especially playground
target_include_directories(
  perf 
  PRIVATE 
  "${CMAKE_SOURCE_DIR}/.."
)

