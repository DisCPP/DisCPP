#
#	DisC++
#
cmake_minimum_required (VERSION 3.6)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake;${CMAKE_MODULE_PATH}")
set(EXTERNAL_INSTALL_LOCATION "${PROJECT_SOURCE_DIR}/thirdparty")
project(discpp)

# Set options for IXWebsocket
set(USE_TLS TRUE)

# Set options for RapidJSON
set(RAPIDJSON_BUILD_DOC OFF)
set(RAPIDJSON_BUILD_EXAMPLES OFF)
set(RAPIDJSON_BUILD_TESTS OFF)
set(RAPIDJSON_BUILD_THIRDPARTY_GTEST OFF)
set(RAPIDJSON_HAS_STDSTRING ON)
set(GTESTSRC_FOUND FALSE)

# Build static libs
set(CURL_STATICLIB ON)
set(BUILD_SHARED_LIBS OFF)

# Set options for CPR/Curl
set(BUILD_CPR_TESTS OFF)
set(BUILD_CPR_TESTS_SSL OFF)
set(BUILD_CURL_EXE OFF)

# Set TLS library for CPR and also IXWebsocket
if(WIN32) # Use MBedTLS for Windows
	set(USE_WINSSL ON)
	set(USE_OPENSSL OFF)
	#[[set(CMAKE_USE_WINSSL OFF)
	set(CMAKE_USE_OPENSSL OFF)
	set(CMAKE_USE_MBEDTLS ON)

	set(SSL_ENABLED ON CACHE INTERNAL "" FORCE)
	set(CURL_CA_PATH "auto" CACHE INTERNAL "" FORCE)]]

	# For IXWebsocket
	set(USE_MBED_TLS ON)
	set(USE_OPEN_SSL OFF)
elseif(APPLE) # Use secure transport for Apple
	set(CMAKE_USE_SECTRANSP ON)
	set(CMAKE_USE_OPENSSL OFF)
	set(USE_OPENSSL OFF)

	# For IXWebsocket
	set(USE_SECURE_TRANSPORT ON)
	set(USE_OPEN_SSL OFF)
else() # Use OpenSSL for anything else.
	set(USE_OPENSSL ON)

	set(USE_OPEN_SSL ON) # For IXWebsocket
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