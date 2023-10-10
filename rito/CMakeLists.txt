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
    src/platform/linux/lcuProcessHandlerImplLinux.cpp
    src/platform/windows/lcuProcessHandlerImplWindows.cpp
)

target_include_directories(rito PUBLIC "inc/")


if (LINUX)
    target_include_directories(rito PRIVATE "src/platform/linux/inc/")
elseif (WIN32)
    target_include_directories(rito PRIVATE "src/platform/windows/inc/")
endif()

target_include_directories(rito PRIVATE "src/inc/")
target_include_directories(rito PRIVATE "inc/rito/")

target_link_libraries(rito PRIVATE
    Poco::Net
    Poco::JSON
    Poco::Foundation
    Poco::NetSSL
)
