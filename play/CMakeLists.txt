cmake_minimum_required(VERSION 3.16)

project("playrobust")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

add_subdirectory(learn)
add_subdirectory(robust )
add_subdirectory(app)