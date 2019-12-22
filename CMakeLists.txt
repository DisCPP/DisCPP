#CMakeLists.txt
cmake_minimum_required (VERSION 3.6)
project(dpp)

set(CMAKE_TOOLCHAIN_FILE C:/vcpkg/scripts/buildsystems/vcpkg.cmake)
set(USE_TLS 1)

find_package(nlohmann_json CONFIG REQUIRED)
find_package(cpr REQUIRED)
find_package(ZLIB REQUIRED)

file(GLOB_RECURSE source_list src/*.cpp)
add_library(discordpp STATIC ${source_list})

target_include_directories(discordpp PUBLIC include)

find_library(MBEDTLS_LIBRARY mbedtls)
if (NOT MBEDTLS_LIBRARY)
    message(FATAL_ERROR "mbedtls library not found")
endif()

find_library(MBEDCRYPTO_LIBRARY mbedcrypto)
if (NOT MBEDCRYPTO_LIBRARY)
    message(FATAL_ERROR "mbedcrypto library not found")
endif()

find_library(MBEDX509_LIBRARY mbedx509)
if (NOT MBEDX509_LIBRARY)
    message(FATAL_ERROR "mbedx509 library not found")
endif()

find_path(IXWEBSOCKET_INCLUDE_DIR ixwebsocket/IXWebSocket.h)
if (NOT IXWEBSOCKET_INCLUDE_DIR)
    message(FATAL_ERROR "ixwebsocket include directory not found")
endif()

find_library(IXWEBSOCKET_LIBRARY ixwebsocket)
if (NOT IXWEBSOCKET_LIBRARY)
    message(FATAL_ERROR "ixwebsocket library not found")
endif()

add_library(ixwebsocket INTERFACE)
target_include_directories(ixwebsocket INTERFACE ${IXWEBSOCKET_INCLUDE_DIR})
target_link_libraries(ixwebsocket INTERFACE ${IXWEBSOCKET_LIBRARY} ${MBEDTLS_LIBRARY} ${MBEDCRYPTO_LIBRARY} ${MBEDX509_LIBRARY} ZLIB::ZLIB)
add_library(ixwebsocket::ixwebsocket ALIAS ixwebsocket)

target_link_libraries(discordpp PRIVATE cpr)
target_link_libraries(discordpp PRIVATE ZLIB::ZLIB)
target_link_libraries(discordpp PUBLIC nlohmann_json::nlohmann_json)
target_link_libraries(discordpp PUBLIC ixwebsocket::ixwebsocket)

set_target_properties(discordpp PROPERTIES CXX_STANDARD 17 CXX_EXTENSIONS OFF)

add_executable(bot_example examples/basic/main.cpp)

target_link_libraries(bot_example PUBLIC discordpp)