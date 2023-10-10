set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

find_package(Poco REQUIRED COMPONENTS Net JSON Foundation NetSSL)

add_library(rito STATIC
    src/httpsRestClient.cpp
    src/httpsWebsocketClient.cpp
    src/riotRestHandler.cpp
    src/lcuRestHandler.cpp
    src/lcuWebsocketHandler.cpp
    src/lcuWampHandler.cpp
    src/lcuProcessHandler.cpp
    src/lcuProcessHandlerHelpers.cpp
)

if (LINUX)
    target_include_directories(rito PRIVATE src/platform/linux/inc/)
    target_sources(rito PRIVATE
        src/platform/linux/lcuProcessHandlerImplLinux.cpp
    )
elseif (WIN32)
    target_include_directories(rito PRIVATE src/platform/windows/inc/)
    target_sources(rito PRIVATE
        src/platform/windows/lcuProcessHandlerImplLinux.cpp
    )
endif()

target_include_directories(rito PRIVATE "src/inc/")

# Public headers visible internally without rito/ prefix
target_include_directories(rito PRIVATE "inc/rito/")

target_include_directories(rito PUBLIC "inc/")


target_link_libraries(rito PRIVATE
    Poco::Net
    Poco::JSON
    Poco::Foundation
    Poco::NetSSL
)
