project("playplay_base")

find_package(spdlog CONFIG REQUIRED)
find_package(boost_asio CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# set sources for playground_core
set(SOURCE 
  runner/thread_runner.cpp
  runner.cpp
)

# set library target with sources
add_library(play_net ${SOURCE})

if (MSVC)
  target_compile_options(play_net PRIVATE /MTd /utf-8)
else()
  target_compile_options(play_net PRIVATE -g -fstack-protector-all)
endif()

# add include path
target_include_directories(play_net PRIVATE "${CMAKE_SOURCE_DIR}/..")

# add link libaries
target_link_libraries(
  play_net 
  PRIVATE 
  spdlog::spdlog 
  nlohmann_json::nlohmann_json
)
