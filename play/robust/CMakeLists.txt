# disable warning on checked_iterator 
add_compile_definitions(_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING=1)

# adding following line causes an compile error
#add_compile_definitions(_WIN32_WINNT=0x060)

add_subdirectory(base)
add_subdirectory(net)
add_subdirectory(test)