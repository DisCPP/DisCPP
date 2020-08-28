#
#	DisC++
#
cmake_minimum_required (VERSION 3.6)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake;${CMAKE_MODULE_PATH}")
set(EXTERNAL_INSTALL_LOCATION "${PROJECT_SOURCE_DIR}/thirdparty")
project(discpp)

# Set options for IXWebsocket
set(USE_TLS TRUE)
set(USE_OPEN_SSL TRUE)

# Set options for RapidJson
set(RAPIDJSON_HAS_STDSTRING 1)

# Set options for C++ standard lib.
set(__STDC_WANT_LIB_EXT1__ 1)

# Set default options
option(USE_RAPID "Uses rapidjson for json parsing." ON)
option(USE_SIMD "Uses simdjson for json parsing. - NOT YET SUPPORTED" OFF)
option(USE_FMT "Uses fmt for logger - NOT YET SUPPORTED" OFF)
option(BUILD_EXAMPLES "Build example bots." OFF)
option(BUILD_TESTS "Build unit tests." OFF)

# Find dependencies
if (USE_FMT)
	find_package(fmt REQUIRED)
	add_compile_definitions(FMT_SUPPORT)
else()
	add_compile_definitions(IOSTREAM_SUPPORT)
endif()

if (USE_RAPID)
	find_package(RapidJSON CONFIG REQUIRED)
	add_compile_definitions(RAPIDJSON_BACKEND)
elseif(USE_SIMD)
	find_package(simdjson CONFIG REQUIRED)
	add_compile_definitions(SIMDJSON_BACKEND)
endif()

find_package(OpenSSL REQUIRED)
find_package(cpr REQUIRED)
find_package(CURL CONFIG REQUIRED)
find_package(ZLIB REQUIRED)
add_subdirectory(thirdparty/IXWebSocket)

# Link sources
file(GLOB_RECURSE source_list src/*.cpp)
add_library(discpp STATIC ${source_list})

# Link headers
target_include_directories(discpp PUBLIC include PRIVATE include/discpp)

# Required for windows support
if (WIN32)
	target_link_libraries(discpp PUBLIC wsock32 ws2_32 shlwapi)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	if (USE_TLS)
		target_link_libraries(discpp PUBLIC Crypt32)
	endif()
endif()

# Link dependencies
if (FMT_ENABLED)
	target_link_libraries(discpp PUBLIC fmt)
endif()

if (USE_RAPID)
	target_include_directories(discpp PUBLIC $<BUILD_INTERFACE:${RAPIDJSON_INCLUDE_DIRS}>)
elseif(USE_SIMD)
	target_link_libraries(discpp PUBLIC simdjson)
endif()

target_link_libraries(discpp PUBLIC ZLIB::ZLIB)
target_link_libraries(discpp PUBLIC CURL::libcurl)
target_link_libraries(discpp PUBLIC OpenSSL::SSL OpenSSL::Crypto)
target_link_libraries(discpp PUBLIC $<BUILD_INTERFACE:ixwebsocket>)
#target_include_directories(discpp PUBLIC $<BUILD_INTERFACE:${IXWEBSOCKET_HEADERS}>)
target_link_libraries(discpp PUBLIC cpr)

# Build unit tests
if (BUILD_TESTS)
    add_subdirectory(tests)
endif()

# Build examples
if (BUILD_EXAMPLES)
	add_subdirectory(examples/pingbot)
	add_subdirectory(examples/serverinfo)
endif()

# Set properties
set_target_properties(discpp PROPERTIES CXX_STANDARD 17 CXX_EXTENSIONS OFF)