cmake_minimum_required(VERSION 3.23)

project(Rito
    VERSION 0.1.0
    LANGUAGES CXX
)

find_package(Poco REQUIRED COMPONENTS Net JSON Foundation NetSSL)

add_library(rito
    src/https_rest_client.cpp
    src/https_websocket_client.cpp
    src/riot_rest_handler.cpp
    src/lcu_rest_handler.cpp
    src/lcu_websocket_handler.cpp
    src/lcu_wamp_handler.cpp
    src/lcu_process_handler.cpp
    src/lcu_process_handler_helpers.cpp
)

add_library(rito::rito ALIAS rito)

set_target_properties(rito PROPERTIES
    SOVERSION ${PROJECT_VERSION_MAJOR}
    VERSION ${PROJECT_VERSION}
)

target_compile_features(rito PUBLIC cxx_std_20)

if (LINUX)
    target_include_directories(rito PRIVATE src/platform/linux/include)
    target_sources(rito PRIVATE
        src/platform/linux/lcu_process_handler_impl_linux.cpp
    )
elseif (WIN32)
    target_include_directories(rito PRIVATE src/platform/windows/include)
    target_sources(rito PRIVATE
        src/platform/windows/lcu_process_handler_impl_windows.cpp
    )
endif()

target_include_directories(rito PRIVATE src/include)

# Public headers visible internally without rito/ prefix
target_include_directories(rito PRIVATE include/rito)

target_include_directories(rito
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(rito PRIVATE
    Poco::Net
    Poco::JSON
    Poco::Foundation
    Poco::NetSSL
)

include(GNUInstallDirs)
set(rito_INSTALL_CMAKEDIR
    ${CMAKE_INSTALL_LIBDIR}/cmake/rito
    CACHE STRING "Path to rito cmake files"
)
install(TARGETS rito
    EXPORT ritoTargets

    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT rito_Runtime
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT rito_Runtime
        NAMELINK_COMPONENT rito_Development
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT rito_Development
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/rito
    DESTINATION include
)

install(EXPORT ritoTargets
    FILE ritoTargets.cmake
    NAMESPACE rito::
    DESTINATION ${rito_INSTALL_CMAKEDIR}
    COMPONENT rito_Development
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/ritoConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/ritoConfig.cmake
    INSTALL_DESTINATION
        ${rito_INSTALL_CMAKEDIR}
)

write_basic_package_version_file(ritoConfigVersion.cmake
    VERSION ${rito_VERSION}
    COMPATIBILITY SameMajorVersion
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/ritoConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/ritoConfigVersion.cmake
    DESTINATION ${rito_INSTALL_CMAKEDIR}
)
