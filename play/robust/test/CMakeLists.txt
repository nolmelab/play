project("test_base")

# find vcpkg packages
find_package(spdlog CONFIG REQUIRED)
find_package(doctest CONFIG REQUIRED)

# add test target
add_executable(
  test
  main.cpp 
  test_logger.cpp
  test_bytes_frame.cpp
  learn_streambuf.cpp
)

if (MSVC)
  target_compile_options(test PRIVATE /MTd)
endif()

# add link libraries. playground_core is available with cmake
target_link_libraries(
  test
  play_robust_base 
  spdlog::spdlog 
  doctest::doctest
)

# add include directories, especially playground
target_include_directories(
  test
  PRIVATE "${CMAKE_SOURCE_DIR}/.."
)

