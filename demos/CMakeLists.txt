function(add_demo name)
  add_executable(${name} "${name}.cpp")

  target_compile_options(${name} PRIVATE -Wall)
  target_include_directories(${name} PRIVATE "${PROJECT_SOURCE_DIR}/src")
  target_link_libraries(${name} PRIVATE rapier)

  set_target_properties(${name}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/rapier/demos"
  ) 
endfunction()

add_demo(hello_world)