set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

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

if (LINUX)
    target_include_directories(rito PRIVATE src/platform/linux/inc/)
    target_sources(rito PRIVATE
        src/platform/linux/lcu_process_handler_impl_linux.cpp
    )
elseif (WIN32)
    target_include_directories(rito PRIVATE src/platform/windows/inc/)
    target_sources(rito PRIVATE
        src/platform/windows/lcu_process_handler_impl_windows.cpp
    )
endif()

target_include_directories(rito PRIVATE src/inc/)

# Public headers visible internally without rito/ prefix
target_include_directories(rito PRIVATE inc/rito/)

target_include_directories(rito PUBLIC inc/)

target_link_libraries(rito PRIVATE
    Poco::Net
    Poco::JSON
    Poco::Foundation
    Poco::NetSSL
)

# include(GNUInstallDirs)
# set(rito_INSTALL_CMAKEDIR
#     ${CMAKE_INSTALL_LIBDIR}/cmake/rito
#     CACHE STRING "Path to rito cmake files"
# )
# install(TARGETS rito
#     EXPORT ritoTargets
#
#     INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
#     RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
#         COMPONENT rito_Runtime
#     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#         COMPONENT rito_Runtime
#         NAMELINK_COMPONENT rito_Development
#     ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
#         COMPONENT rito_Development
# )
#
# install(EXPORT ritoTargets
#     FILE ritoTargets.cmake
#     NAMESPACE rito::
#     DESTINATION ${rito_INSTALL_CMAKEDIR}
#     COMPONENT rito_Development
# )
