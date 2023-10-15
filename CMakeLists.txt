cmake_minimum_required(VERSION 3.23)

option(BUILD_SHARED_LIBS "Build shared library" ON)
option(BUILD_TESTING "Build unit and block tests" OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to Release as none was specified")
    set(CMAKE_BUILD_TYPE Release)
endif()

project(Rito
    VERSION 0.1.0
    LANGUAGES CXX
)

include(CTest)
if(BUILD_TESTING)
    find_package(GTest REQUIRED)
endif()

add_subdirectory(src)

include (CPack)
