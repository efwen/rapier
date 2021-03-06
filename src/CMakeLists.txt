set(SRC_FILES pch.cpp)
set(SRC_FILES ${SRC_FILES} core/core.cpp core/event.cpp core/window.cpp)
set(SRC_FILES ${SRC_FILES} input/keyboard.cpp)
set(SRC_FILES ${SRC_FILES} log/log.cpp)
set(SRC_FILES ${SRC_FILES} util/version.cpp util/uuid.cpp)

if(${WIN32})
  message(STATUS "Adding Windows Platform Files...")
  set(SRC_FILES ${SRC_FILES} platform/win32_window.cpp platform/win32_keyboard.cpp)
else()
  message(FATAL_ERROR "OS not supported!") 
endif()

add_library(rapier ${SRC_FILES})

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(rapier PUBLIC -W3)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(rapier PUBLIC -Wall -Wextra -Wpedantic)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(rapier PUBLIC -Wall -Wextra -Wpedantic)
endif()

target_precompile_headers(rapier PRIVATE pch.hpp)
target_include_directories(rapier PRIVATE fmt::fmt "${CMAKE_PROJECT_DIRECTORY}")
target_link_libraries(rapier fmt::fmt)

set_target_properties(rapier
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/rapier/lib"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/rapier/lib"
) 