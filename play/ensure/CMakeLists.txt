project("playrobust_base")

find_package(spdlog CONFIG REQUIRED)

# set sources for playground_core
set(SOURCE 
  ensure.cpp
  bot.cpp
  act_composite.cpp
  act_factory.cpp
  act_parallel.cpp
  act_serial.cpp
  act.cpp
  bot.cpp
  flow.cpp
)

# set library target with sources
add_library(play_robust_ensure ${SOURCE})

if (MSVC)
  target_compile_options(play_robust_ensure PRIVATE /MTd)
endif()

# add include path
target_include_directories(play_robust_ensure PRIVATE "${CMAKE_SOURCE_DIR}/..")

# add link libaries
target_link_libraries(play_robust_ensure spdlog::spdlog)
