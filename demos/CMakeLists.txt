function(add_demo name)
  add_executable(${name} "${name}.cpp")

  target_link_libraries(${name} PRIVATE rapier)

  set_target_properties(${name}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/rapier/demos"
  ) 
endfunction()

add_demo(hello_world)