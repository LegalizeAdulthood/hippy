find_package(GTest CONFIG REQUIRED)
include(GoogleTest)

add_executable(test-asm
    test-codes.cpp
    test-parser.cpp
)
target_link_libraries(test-asm PUBLIC asm-lib GTest::gtest_main)
set_target_properties(test-asm PROPERTIES FOLDER Tests)
gtest_discover_tests(test-asm)
