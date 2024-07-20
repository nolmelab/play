project("play_robust_db")

find_package(spdlog CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(unixodbc REQUIRED)

# set sources for playground_core
set(SOURCE 
  nanodbc.cpp
)

# set library target with sources
add_library(play_robust_db ${SOURCE})

if (MSVC)
  target_compile_options(play_robust_db PRIVATE /MTd /utf-8)
else()
  target_compile_options(play_robust_db PRIVATE -g -fstack-protector-all)
endif()

# add include path
target_include_directories(play_robust_db PRIVATE "${CMAKE_SOURCE_DIR}/..")

# add link libaries
target_link_libraries(
  play_robust_db
  PRIVATE 
  spdlog::spdlog 
  nlohmann_json::nlohmann_json
  UNIX::odbc
)