add_executable(Rito-test
    test_placeholder.cpp
)

if (LINUX)
    target_sources(Rito-test
        PRIVATE
            test_lcu_process_handler_linux.cpp
    )
elseif (WIN32)
    target_sources(Rito-test
        PRIVATE
    )
endif()

target_link_libraries(Rito-test
    PRIVATE
        Rito
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(Rito-test
    WORKING_DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR})
