cmake_minimum_required(VERSION 3.16)

project("playrobust")

# c++20 is not completely supported by all compilers and platforms
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

add_subdirectory(robust )
add_subdirectory(app)
add_subdirectory(external)