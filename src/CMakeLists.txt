add_library(rapier core/core.cpp core/window.cpp util/version.cpp log/log.cpp platform/win32_window.cpp)

target_compile_options(rapier PRIVATE -Wall)
target_precompile_headers(rapier PRIVATE pch.hpp)
target_include_directories(rapier PRIVATE fmt::fmt "${CMAKE_PROJECT_DIRECTORY}")
target_link_libraries(rapier fmt::fmt)

set_target_properties(rapier
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/rapier/lib"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/rapier/lib"
) 