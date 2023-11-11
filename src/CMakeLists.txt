find_package(Poco
    REQUIRED
    COMPONENTS
        Net
        JSON
        Foundation
        NetSSL
)

add_library(Rito
    https_rest_client.cpp
    https_websocket_client.cpp
    riot_rest_handler.cpp
    lcu_rest_handler.cpp
    lcu_websocket_handler.cpp
    lcu_wamp_handler.cpp
    lcu_process_handler.cpp
    lcu_process_handler_helpers.cpp
)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)

add_library(Rito::Rito ALIAS Rito)

set_target_properties(Rito
    PROPERTIES
        SOVERSION ${PROJECT_VERSION_MAJOR}
        VERSION ${PROJECT_VERSION}

        DEBUG_POSTFIX d
)

include(GenerateExportHeader)
generate_export_header(Rito)

target_compile_features(Rito PUBLIC cxx_std_20)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang"
        OR
    CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(Rito PRIVATE
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
    )
elseif(CMAKE_CXX_COMPILER_IR STREQUAL "MSVC")
    target_compile_options(Rito PRIVATE
        /Wall
    )
endif()

if (LINUX)
    target_include_directories(Rito
        PRIVATE
            platform/linux/include
    )
    target_sources(Rito
        PRIVATE
            platform/linux/lcu_process_handler_impl_linux.cpp
    )
elseif (WIN32)
    target_include_directories(Rito
        PRIVATE
            platform/windows/include
    )
    target_sources(Rito
        PRIVATE
            platform/windows/lcu_process_handler_impl_windows.cpp
    )
endif()

target_include_directories(Rito
    PRIVATE
        include  # Internal headers
        ${Rito_SOURCE_DIR}/include/rito # Public headers visible internally without rito/ prefix
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<BUILD_INTERFACE:${Rito_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(Rito
    PRIVATE
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

    INCLUDES
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    RUNTIME
        DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY ${Rito_SOURCE_DIR}/include/rito
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/rito_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/rito
)

install(EXPORT RitoTargets
    FILE RitoTargets.cmake
    NAMESPACE Rito::
    DESTINATION ${Rito_INSTALL_CMAKEDIR}
)

include(CMakePackageConfigHelpers)

configure_package_config_file(
    ${Rito_SOURCE_DIR}/cmake/RitoConfig.cmake.in
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
