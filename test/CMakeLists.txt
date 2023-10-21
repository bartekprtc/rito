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
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Coverage
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_libraries(Rito
        PRIVATE
            gcov
    )
    set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
    include(CodeCoverage)

    append_coverage_compiler_flags_to_target(Rito)
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -O0")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
    setup_target_for_coverage_lcov(
        NAME
            Rito-coverage
        EXECUTABLE
            ctest
        DEPENDENCIES
            Rito-test
            Rito
        EXCLUDE
            /usr/*
    )
endif()
