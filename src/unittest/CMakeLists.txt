add_executable(example-test
    example_test.cpp
)

target_link_libraries(
    example-test
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(example-test)
