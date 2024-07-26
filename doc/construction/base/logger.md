# logger 

simple is the best. 

- daily async logger. 
- copied from playground 

## build 

- playplay_base as a library target
- test project uses playplay_base 


### build error: 

[1] cmake cannot find spdlog. 

vscode can debug, vs2022 also can debug cmake errors. 
but it does not help much. 

- cmake generation option and installed vcpkg triplet does not match 
  - how to modify -G and -A 
  - choose Visual Studio amd64 compiler
    - view cmake shows Configure section with the compiler selection

[2] checked_array_iterator warning 

- define _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING 

[3] vs2022 language pack 

- default to english. vscode cannot display euc-kr. 
- still shows some invisible characters

[4] dynamic link 

- set static 
- target_compile_options(play_base PRIVATE /MT)
  - still error with _ITERATOR_DEBUG_LEVEL and RuntimeLibrary 
  - /MTd works 
  - needs to make it optional for MSVC 
   
[5] debugging

- lldb or gdb works on linux with c/c++ extensions 
- windows? it seems not simple. 
  - windbg extension 
  - it works using the new windbg, not classic one. 







