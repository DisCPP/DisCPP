# CMakeLists.txt
cmake_minimum_required (VERSION 3.6)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake;${CMAKE_MODULE_PATH}")
set(USE_MBED_TLS ON)
project(discpp)

option(BUILD_EXAMPLES "Build example bots" OFF)
option(BUILD_TESTS "Build unit tests" OFF)

find_package(RapidJSON CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(cpr REQUIRED)
find_package(CURL QUIET COMPONENTS libcurl CONFIG)
find_package(ZLIB REQUIRED)
find_package(MbedTLS REQUIRED)
find_package(ixwebsocket REQUIRED)

link_libraries(z)
link_libraries(ssl)
link_libraries(crypto)

# Add source files
file(GLOB_RECURSE source_list src/*.cpp)
add_library(discpp STATIC ${source_list} ${events_source_list})

target_include_directories(discpp PUBLIC include PRIVATE include/discpp)
target_include_directories(discpp PUBLIC ${RAPIDJSON_INCLUDE_DIRS})

if (ZLIB_FOUND)
	include_directories(${ZLIB_INCLUDE_DIRS})
	target_link_libraries(discpp PUBLIC ${ZLIB_LIBRARIES})
endif()

if (CURL_FOUND)
	include_directories(${CURL_INCLUDE_DIR})
	target_link_libraries(discpp PUBLIC ${CURL_LIBRARIES})
	target_link_libraries(discpp PUBLIC CURL::libcurl)
endif()

target_link_libraries(discpp PUBLIC ${MBEDTLS_LIBRARY})

if (OPENSSL_FOUND)
	include_directories(${OPENSSL_INCLUDE_DIR})
	target_link_libraries(discpp PUBLIC ${OPENSSL_LIBRARIES})
	target_link_libraries(discpp PUBLIC OpenSSL::Crypto)
endif()

target_link_libraries(discpp PUBLIC cpr)

if (WIN32)
	target_link_libraries(discpp PUBLIC wsock32 ws2_32 shlwapi)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	if (USE_TLS)
		target_link_libraries(discpp PUBLIC Crypt32)
	endif()
endif()

if (UNIX)
	find_package(Threads)
	target_link_libraries(discpp PUBLIC ${CMAKE_THREAD_LIBS_INIT})
	link_libraries(pthread)
	link_libraries(dl)
endif()

target_link_libraries(discpp PUBLIC ${IXWEBSOCKET_LIBRARY})

# Unit tests
if (BUILD_TESTS)
    add_subdirectory(tests)
endif()

# Examples
if (BUILD_EXAMPLES)
	add_subdirectory(examples/pingbot)
	add_subdirectory(examples/serverinfo)
endif()

set_target_properties(discpp PROPERTIES CXX_STANDARD 17 CXX_EXTENSIONS OFF)