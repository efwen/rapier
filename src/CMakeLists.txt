add_library(rapier core.cpp version.cpp log.cpp)

target_compile_options(rapier PRIVATE -Wall)
target_precompile_headers(rapier PRIVATE pch.hpp)
target_include_directories(rapier PRIVATE fmt::fmt)
target_link_libraries(rapier fmt::fmt)

set_target_properties(rapier
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/rapier/lib"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/rapier/lib"
) 