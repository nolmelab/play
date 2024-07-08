# disable warning on checked_iterator 
add_compile_definitions(_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING=1)

add_subdirectory(base)
add_subdirectory(test)