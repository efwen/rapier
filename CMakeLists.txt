cmake_minimum_required(VERSION 3.16)

project(rapier VERSION 0.0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Generate a compile_commands.json file

include(FetchContent)

FetchContent_Declare(
  fmtlib
  GIT_REPOSITORY  https://github.com/fmtlib/fmt.git
  GIT_TAG         7.1.3
)
FetchContent_MakeAvailable(fmtlib)

add_subdirectory(src)
add_subdirectory(demos)