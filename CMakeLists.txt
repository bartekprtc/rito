cmake_minimum_required(VERSION 3.23)

project(Rito
    VERSION 0.1.0
    LANGUAGES CXX
)

find_package(Poco REQUIRED COMPONENTS Net JSON Foundation NetSSL)

add_library(Rito
    src/https_rest_client.cpp
    src/https_websocket_client.cpp
    src/riot_rest_handler.cpp
    src/lcu_rest_handler.cpp
    src/lcu_websocket_handler.cpp
    src/lcu_wamp_handler.cpp
    src/lcu_process_handler.cpp
    src/lcu_process_handler_helpers.cpp
)

add_library(Rito::Rito ALIAS Rito)

set_target_properties(Rito PROPERTIES
    SOVERSION ${PROJECT_VERSION_MAJOR}
    VERSION ${PROJECT_VERSION}
    DEBUG_POSTFIX d
)

target_compile_features(Rito PUBLIC cxx_std_20)

if (LINUX)
    target_include_directories(Rito PRIVATE src/platform/linux/include)
    target_sources(Rito PRIVATE
        src/platform/linux/lcu_process_handler_impl_linux.cpp
    )
elseif (WIN32)
    target_include_directories(Rito PRIVATE src/platform/windows/include)
    target_sources(Rito PRIVATE
        src/platform/windows/lcu_process_handler_impl_windows.cpp
    )
endif()

target_include_directories(Rito PRIVATE src/include)

# Public headers visible internally without rito/ prefix
target_include_directories(Rito PRIVATE include/rito)

target_include_directories(Rito
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(Rito PRIVATE
    Poco::Net
    Poco::JSON
    Poco::Foundation
    Poco::NetSSL
)

include(GNUInstallDirs)
set(Rito_INSTALL_CMAKEDIR
    ${CMAKE_INSTALL_LIBDIR}/cmake/rito
    CACHE STRING "Path to Rito cmake files"
)
install(TARGETS Rito
    EXPORT RitoTargets

    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/rito
    DESTINATION include
)

install(EXPORT RitoTargets
    FILE RitoTargets.cmake
    NAMESPACE Rito::
    DESTINATION ${Rito_INSTALL_CMAKEDIR}
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/RitoConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/RitoConfig.cmake
    INSTALL_DESTINATION
        ${Rito_INSTALL_CMAKEDIR}
)

write_basic_package_version_file(RitoConfigVersion.cmake
    VERSION ${Rito_VERSION}
    COMPATIBILITY SameMajorVersion
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/RitoConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/RitoConfigVersion.cmake
    DESTINATION ${Rito_INSTALL_CMAKEDIR}
)

include (CPack)
