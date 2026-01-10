cmake_minimum_required(VERSION 3.10)
project(rwstd VERSION 1.0.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

message(STATUS "BUILD_SHARED_LIBS: ${BUILD_SHARED_LIBS}")


add_subdirectory(src/Allocator)
add_subdirectory(src/Iterator)
add_subdirectory(src/Vector)
add_subdirectory(scratchpad)


option(BUILD_TESTS "Build unit tests" OFF)



if (BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

