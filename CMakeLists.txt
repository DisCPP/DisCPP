#
#	discpp
#
cmake_minimum_required (VERSION 3.6)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake;${CMAKE_MODULE_PATH}")
set(USE_TLS TRUE)
set(USE_OPEN_SSL TRUE)
project(discpp)

# set options
option(BUILD_EXAMPLES "Build example bots" OFF)
option(BUILD_TESTS "Build unit tests" OFF)

# find dependencies
find_package(RapidJSON CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(cpr REQUIRED)
find_package(CURL CONFIG REQUIRED)
find_package(ZLIB REQUIRED)
add_subdirectory(thirdparty/IXWebSocket)

# required for intexisty to compile this library
# on arch x86_64
if (UNIX)
	link_libraries(z)
	link_libraries(ssl)
	link_libraries(crypto)
endif()

# link sources
file(GLOB_RECURSE source_list src/*.cpp)
add_library(discpp STATIC ${source_list})

# link headers
target_include_directories(discpp PUBLIC include PRIVATE include/discpp)

# required for windows support
if (WIN32)
	target_link_libraries(discpp PUBLIC wsock32 ws2_32 shlwapi)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	if (USE_TLS)
		target_link_libraries(discpp PUBLIC Crypt32)
	endif()
else()
	# required by other operating systems
	find_package(Threads)
	target_link_libraries(discpp PUBLIC ${CMAKE_THREAD_LIBS_INIT})
endif()

# link dependencies
target_include_directories(discpp PUBLIC ${RAPIDJSON_INCLUDE_DIRS})

target_link_libraries(discpp PUBLIC ZLIB::ZLIB)
target_link_libraries(discpp PUBLIC CURL::libcurl)
target_link_libraries(discpp PUBLIC OpenSSL::SSL OpenSSL::Crypto)
target_link_libraries(discpp PUBLIC ixwebsocket)
target_link_libraries(discpp PUBLIC cpr)

# build unit tests
if (BUILD_TESTS)
    add_subdirectory(tests)
endif()

# build examples
if (BUILD_EXAMPLES)
	add_subdirectory(examples/pingbot)
	add_subdirectory(examples/serverinfo)
endif()

# set properties
set_target_properties(discpp PROPERTIES CXX_STANDARD 17 CXX_EXTENSIONS OFF)