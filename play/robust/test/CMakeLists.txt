project("test_base")

add_subdirectory(perf)

# find vcpkg packages_
find_package(spdlog CONFIG REQUIRED)
find_package(doctest CONFIG REQUIRED)
find_package(boost_asio CONFIG REQUIRED)
find_package(unofficial-sodium CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# custom target can execute a command
add_custom_target(
  fb 
  # -o option needs to come before the input files
  # TODO: add dependcy on the generated files (build only when fbs is changed)
  COMMAND flatc --cpp --gen-object-api -o "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/*.fbs" 
)

# add test target
add_executable(
  test
  main.cpp 
  learn_streambuf.cpp
  learn_function_ptr.cpp
  learn_flatbuffers.cpp
  test_app.cpp
  test_codecs.cpp
  test_flatbuffers.cpp
  test_logger.cpp
  test_object_pool.cpp
  test_plain_protocol.cpp
  test_protocol.cpp
  test_runner.cpp
  test_secure_protocol.cpp
  test_stream_protocol.cpp
  test_timer.cpp
)

if (MSVC)
  target_compile_options(test PRIVATE /MTd /utf-8)
else() 
  target_compile_options(test PRIVATE -g -fstack-protector-all)
endif()

# dependency makes a build chain
add_dependencies(test fb)

# add link libraries. playground_core is available with cmake
target_link_libraries(
  test
  play_robust_base 
  play_robust_net
  spdlog::spdlog 
  doctest::doctest
  unofficial-sodium::sodium
  nlohmann_json::nlohmann_json
  pthread
)

# add include directories, especially playground
target_include_directories(
  test
  PRIVATE 
  "${CMAKE_SOURCE_DIR}/.."
)

