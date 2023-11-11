cmake_minimum_required(VERSION 3.25)

option(BUILD_SHARED_LIBS "Build shared library" ON)
option(BUILD_TESTING "Build unit and block tests" OFF)
option(STATIC_ANALYSIS "Run static analysis" OFF)

if(BUILD_TESTING AND STATIC_ANALYSIS)
    message(FATAL_ERROR "BUILD_TESTING and STATIC_ANALYSIS options can't be active at the same time")
endif()

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to Release as none was specified")
    set(CMAKE_BUILD_TYPE Release)
endif()

if(STATIC_ANALYSIS)
    set(CMAKE_CXX_CLANG_TIDY clang-tidy)
endif()

project(Rito
    VERSION 0.1.0
    LANGUAGES CXX
)

add_subdirectory(src)

include(CTest)
if(BUILD_TESTING)
    find_package(GTest REQUIRED)
    add_subdirectory(test)
endif()

include (CPack)
