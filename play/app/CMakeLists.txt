project("playplay_base")

find_package(spdlog CONFIG REQUIRED)

# set sources for playground_core
set(SOURCE 
  empty.cpp
)

# set library target with sources
add_library(play_app ${SOURCE})

if (MSVC)
  target_compile_options(play_app PRIVATE /MTd)
endif()

# add include path
target_include_directories(play_app PRIVATE "${CMAKE_SOURCE_DIR}/..")

# add link libaries
target_link_libraries(play_app spdlog::spdlog)
