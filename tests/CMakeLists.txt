
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

FetchContent_MakeAvailable(googletest)

add_executable(vector_test vector_test.cc)
target_link_libraries(vector_test PRIVATE GTest::gtest_main Vector)

include(GoogleTest)
gtest_discover_tests(vector_test)
