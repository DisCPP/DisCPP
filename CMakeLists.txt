#
#	DisC++
#
cmake_minimum_required (VERSION 3.6)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake;${CMAKE_MODULE_PATH}")
set(EXTERNAL_INSTALL_LOCATION "${PROJECT_SOURCE_DIR}/thirdparty")
project(discpp)

# Set options for IXWebsocket
set(USE_TLS TRUE CACHE BOOL "")

# Set options for RapidJSON
set(RAPIDJSON_BUILD_DOC OFF CACHE BOOL "")
set(RAPIDJSON_BUILD_EXAMPLES OFF CACHE BOOL "")
set(RAPIDJSON_BUILD_TESTS OFF CACHE BOOL "")
set(RAPIDJSON_BUILD_THIRDPARTY_GTEST OFF CACHE BOOL "")
set(RAPIDJSON_HAS_STDSTRING ON CACHE BOOL "")

# Build static libs
set(CURL_STATICLIB ON CACHE BOOL "")
set(BUILD_SHARED_LIBS OFF CACHE BOOL "")

# Set options for CPR/Curl
set(BUILD_CPR_TESTS OFF CACHE BOOL "")
set(BUILD_CURL_EXE OFF CACHE BOOL "")

# Set TLS library for CPR and also IXWebsocket
if(WIN32) # Use MBedTLS for Windows
	set(USE_WINSSL ON CACHE BOOL "")
	set(USE_OPENSSL OFF CACHE BOOL "")
	#[[set(CMAKE_USE_WINSSL OFF)
	set(CMAKE_USE_OPENSSL OFF)
	set(CMAKE_USE_MBEDTLS ON)

	set(SSL_ENABLED ON CACHE INTERNAL "" FORCE)
	set(CURL_CA_PATH "auto" CACHE INTERNAL "" FORCE)]]

	# For IXWebsocket
	set(USE_MBED_TLS ON CACHE BOOL "")
	set(USE_OPEN_SSL OFF CACHE BOOL "")
elseif(APPLE) # Use secure transport for Apple
	set(CMAKE_USE_SECTRANSP ON CACHE BOOL "")
	set(CMAKE_USE_OPENSSL OFF CACHE BOOL "")
	set(USE_OPENSSL OFF CACHE BOOL "")

	# For IXWebsocket
	set(USE_SECURE_TRANSPORT ON CACHE BOOL "")
	set(USE_OPEN_SSL OFF CACHE BOOL "")
else() # Use OpenSSL for anything else.
	set(USE_OPENSSL ON CACHE BOOL "")

	set(USE_OPEN_SSL ON CACHE BOOL "") # For IXWebsocket

	set(CURL_CA_PATH "${CMAKE_CURRENT_SOURCE_DIR}/ca-chain.cert.pem" CACHE INTERNAL "" FORCE)
	set(CURL_CA_BUNDLE "${CMAKE_CURRENT_SOURCE_DIR}/ca-chain.cert.pem" CACHE INTERNAL "" FORCE)
endif()

# Set default options
option(USE_RAPID "Uses RapidJSON for json parsing." ON)
option(USE_SIMD "Uses simdjson for json parsing. - NOT YET SUPPORTED" OFF)
option(USE_FMT "Uses fmt for logger - NOT YET SUPPORTED" OFF)
option(BUILD_EXAMPLES "Build example bots." OFF)
option(BUILD_TESTS "Build unit tests." OFF)

# Find dependencies
if (USE_FMT)
	find_package(fmt REQUIRED)
endif()

if (USE_RAPID)
	add_subdirectory(thirdparty/rapidjson)
	add_compile_definitions(RAPIDJSON_BACKEND)
elseif(USE_SIMD)
	find_package(simdjson CONFIG REQUIRED)
	add_compile_definitions(SIMDJSON_BACKEND)
endif()

add_subdirectory(thirdparty/cpr)
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

	target_compile_definitions(discpp PUBLIC FMT_SUPPORT)
else()
	target_compile_definitions(discpp PUBLIC IOSTREAM_SUPPORT)
endif()

if (USE_RAPID)
	target_include_directories(discpp PUBLIC ${RAPIDJSON_INCLUDE_DIRS})
elseif(USE_SIMD)
	target_link_libraries(discpp PUBLIC simdjson)
endif()

# Set options for C++ standard lib.
target_compile_definitions(discpp PUBLIC __STDC_WANT_LIB_EXT1__)

# I have to do some weird stuff to get Cpr to link
target_compile_definitions(${CPR_LIBRARIES} PUBLIC CURL_STATICLIB)
target_include_directories(discpp PUBLIC ${CPR_INCLUDE_DIRS})
target_link_libraries(discpp PUBLIC ${CPR_LIBRARIES})

target_link_libraries(discpp PUBLIC $<BUILD_INTERFACE:ixwebsocket>)

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
