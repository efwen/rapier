add_library(rapier version.cpp)

target_compile_options(rapier PUBLIC -Wall)
target_include_directories(rapier PUBLIC fmt::fmt ../include)
target_link_libraries(rapier fmt::fmt)

set_target_properties(rapier
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/rapier/lib"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/rapier/lib"
) 