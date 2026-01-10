

add_library(Vector INTERFACE)
target_compile_options(Vector INTERFACE
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

target_include_directories(Vector INTERFACE ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(Vector INTERFACE Iterator)
target_link_libraries(Vector INTERFACE Allocator)
