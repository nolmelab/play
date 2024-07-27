add_custom_target(ensure ALL)
add_custom_target(app ALL)

# 유용한 명령어로 디렉토리를 복사. 폴더로 하려면 /를 붙여햐 함
add_custom_command(
        TARGET ensure POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory 
                ${CMAKE_CURRENT_SOURCE_DIR}/
                ${CMAKE_CURRENT_BINARY_DIR}/
)
