project("playplay_base")

find_package(spdlog CONFIG REQUIRED)

# set sources for playground_core
set(SOURCE
  logger.cpp
  macros.cpp
  uuid.cpp
)

# set library target with sources
add_library(play_base ${SOURCE})

if(MSVC)
  target_compile_options(play_base PRIVATE /MTd)
endif()

# add include path
target_include_directories(play_base PRIVATE "${CMAKE_SOURCE_DIR}/..")

# add link libaries
target_link_libraries(play_base spdlog::spdlog)
