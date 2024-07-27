add_custom_target(bin)

add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/*.json
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_SOURCE_DIR}/*.json
                ${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_command(
        TARGET bin POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/*.json
                ${CMAKE_CURRENT_BINARY_DIR}
)