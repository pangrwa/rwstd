add_executable(test_vector test_vector.cpp)
target_link_libraries(test_vector PRIVATE Vector)

add_executable(test_allocator test_allocator.cpp)
target_link_libraries(test_allocator PRIVATE Allocator)
# target_include_directories(test_vector PRIVATE ${PROJECT_SOURCE_DIR}/src/Vector)

add_executable(test_unordered_map test_unordered_map.cpp)
target_link_libraries(test_unordered_map PRIVATE UnorderedMap)

