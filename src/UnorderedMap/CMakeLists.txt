
add_library(UnorderedMap INTERFACE)
target_compile_options(UnorderedMap INTERFACE
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

target_include_directories(UnorderedMap INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(UnorderedMap INTERFACE Iterator)
target_link_libraries(UnorderedMap INTERFACE Allocator)
