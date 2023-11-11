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

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang"
        OR
    CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(Rito-test PRIVATE
        -Wextra
        -Wall
        -Werror
        -Wfloat-equal
        -Wshadow
        -Wpointer-arith
        -Wcast-align
        -Wstrict-overflow=5
        -Wwrite-strings
        -Wcast-qual
        -Wswitch-enum
        -Wconversion
        -Wunreachable-code
        -Wformat=2
        -Wuninitialized
        -Winit-self
        -pedantic

        -fsanitize=address
        -fsanitize=leak
        -fsanitize=undefined
    )
    target_link_options(Rito-test PRIVATE
        -fsanitize=address
        -fsanitize=undefined
    )
endif()

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
