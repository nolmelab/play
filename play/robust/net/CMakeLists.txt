project("playrobust_base")

find_package(spdlog CONFIG REQUIRED)
find_package(boost CONFIG REQUIRED)

# set sources for playground_core
set(SOURCE 
  protocol/length_delimited.cpp
)

# set library target with sources
add_library(play_robust_net ${SOURCE})

if (MSVC)
  target_compile_options(play_robust_net PRIVATE /MTd)
endif()

# add include path
target_include_directories(play_robust_net PRIVATE "${CMAKE_SOURCE_DIR}/..")

# add link libaries
target_link_libraries(play_robust_net spdlog::spdlog)
