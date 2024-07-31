
add_custom_target(
  alpha_fb ALL
  COMMAND flatc --cpp --scoped-enums --gen-object-api -o "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/*.fbs" 
)
