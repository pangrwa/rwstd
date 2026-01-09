add_library(Iterator INTERFACE)
target_compile_options(Iterator INTERFACE
  -Wall
  -Wextra
  -Wpedantic
  -Werror
  -Wshadow
  -Wundef
  -Wcast-align
  -Wformat=2
  -Wconversion
  -Wsign-conversion
  -Wnull-dereference
  -Wdouble-promotion
  -Wimplicit-fallthrough
)
target_include_directories(Iterator INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
