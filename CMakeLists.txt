cmake_minimum_required(VERSION 3.20)
project(SimpleDataServer VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(WITH_OPENSSL "Build with OpenSSL support" ON)

if(CMAKE_VERSION VERSION_LESS "3.24")
    set(CMAKE_CXX_STANDARD 20)
endif()

find_package(Threads REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/lib/nlohmann)
include_directories(${PROJECT_SOURCE_DIR}/lib/uWebSockets/src)
include_directories(${PROJECT_SOURCE_DIR}/lib/uWebSockets/uSockets/src)

set(SOURCES
    src/main.cpp
    src/server/data_server.cpp
    src/handlers/api_handler.cpp
    src/storage/file_manager.cpp
)

set(HEADERS
    src/server/data_server.hpp
    src/handlers/api_handler.hpp
    src/storage/file_manager.hpp
)

add_subdirectory(${PROJECT_SOURCE_DIR}/lib/uWebSockets/uSockets)

add_executable(simpledataserver ${SOURCES} ${HEADERS})

target_link_libraries(simpledataserver PRIVATE
    uSockets
    Threads::Threads
)

if(WITH_OPENSSL)
    find_package(OpenSSL REQUIRED)
    target_link_libraries(simpledataserver PRIVATE OpenSSL::SSL OpenSSL::Crypto)
endif()

target_compile_options(simpledataserver PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

target_compile_definitions(simpledataserver PRIVATE
    LIBUS_USE_OPENSSL=$<BOOL:${WITH_OPENSSL}>
)

install(TARGETS simpledataserver DESTINATION bin)
