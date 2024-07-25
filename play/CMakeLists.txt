cmake_minimum_required(VERSION 3.16)

project(play)

set (CMAKE_CXX_STANDARD 17)

# disable warning on checked_iterator 
add_compile_definitions(_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING=1)

# adding following line causes an compile error
#add_compile_definitions(_WIN32_WINNT=0x060)

add_subdirectory(base)
add_subdirectory(net)
add_subdirectory(app)
add_subdirectory(ensure)
add_subdirectory(db)
add_subdirectory(sample)
add_subdirectory(test)