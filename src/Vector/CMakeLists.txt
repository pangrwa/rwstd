

add_library(Vector INTERFACE)

target_include_directories(Vector INTERFACE ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(Vector INTERFACE Iterator)
