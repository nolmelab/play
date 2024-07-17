project("test_base")

# find vcpkg packages
find_package(spdlog CONFIG REQUIRED)
find_package(doctest CONFIG REQUIRED)
find_package(boost_asio CONFIG REQUIRED)
find_package(unofficial-sodium CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# add test target
add_executable(
  test
  main.cpp 
  test_logger.cpp
  #test_client_server.cpp
  test_codecs.cpp
  test_object_pool.cpp
  test_plain_protocol.cpp
  test_protocol.cpp
  test_runner.cpp
  test_stream_protocol.cpp
  test_timer.cpp
  learn_streambuf.cpp
  learn_function_ptr.cpp
)

if (MSVC)
  target_compile_options(test PRIVATE /MTd /utf-8)
else() 
  target_compile_options(test PRIVATE -g -fstack-protector-all)
endif()

# add link libraries. playground_core is available with cmake
target_link_libraries(
  test
  play_robust_base 
  play_robust_net
  spdlog::spdlog 
  doctest::doctest
  unofficial-sodium::sodium
  nlohmann_json::nlohmann_json
)

# add include directories, especially playground
target_include_directories(
  test
  PRIVATE 
  "${CMAKE_SOURCE_DIR}/.."
)

