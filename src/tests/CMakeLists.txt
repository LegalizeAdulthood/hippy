find_package(GTest CONFIG REQUIRED)
include(GoogleTest)

add_executable(test-hippy basic.cpp)
target_link_libraries(test-hippy GTest::gtest_main)
set_target_properties(test-hippy PROPERTIES FOLDER Tests)
gtest_discover_tests(test-hippy)
