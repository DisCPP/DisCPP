#
#	discpp
#
cmake_minimum_required (VERSION 3.6)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake;${CMAKE_MODULE_PATH}")
set(EXTERNAL_INSTALL_LOCATION "${PROJECT_SOURCE_DIR}/thirdparty")
set(USE_TLS TRUE)
set(USE_OPEN_SSL TRUE)
project(discpp)

# set options
option(USE_RAPID "Uses rapidjson for json parsing" ON)
option(USE_SIMD "Uses simdjson for json parsing" OFF)
option(USE_FMT "Uses fmt for logger" OFF)
option(BUILD_EXAMPLES "Build example bots" OFF)
option(BUILD_TESTS "Build unit tests" OFF)

# find dependencies
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

if (USE_FMT)
	if (NOT fmt_FOUND)
		ExternalProject_Add(fmt
				GIT_REPOSITORY https://github.com/fmtlib/fmt
				CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
		)
	endif()
	add_subdirectory(thirdparty/fmt)
endif()

if (USE_RAPID)
	if (NOT RapidJSON_FOUND)
		ExternalProject_Add(RapidJSON
				GIT_REPOSITORY https://github.com/Tencent/rapidjson
				CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
		)
		add_subdirectory(thirdparty/rapidjson)
	endif()
elseif(USE_SIMD)
	if (NOT simdjson_FOUND)
		ExternalProject_Add(simdjson
				GIT_REPOSITORY https://github.com/simdjson/simdjson
				CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
		)
		add_subdirectory(thirdparty/simdjson)
	endif()
endif()

if (NOT OpenSSL_FOUND)
	ExternalProject_Add(OpenSSL
			GIT_REPOSITORY https://github.com/openssl/openssl
			CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
	)
	add_subdirectory(thirdparty/openssl)
endif()

if (NOT cpr_FOUND)
	ExternalProject_Add(cpr
			GIT_REPOSITORY https://github.com/whoshuu/cpr
			CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
	)
	add_subdirectory(thirdparty/cpr)
endif()

if (NOT CURL_FOUND)
	ExternalProject_Add(curl
			GIT_REPOSITORY https://github.com/curl/curl
			CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
	)
	add_subdirectory(thirdparty/curl)
endif()

if (NOT ZLIB_FOUND)
	ExternalProject_Add(zlib
			GIT_REPOSITORY https://github.com/madler/zlib
			CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
	)
	add_subdirectory(thirdparty/zlib)
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
	# find_package(Threads)
	# target_link_libraries(discpp PUBLIC ${CMAKE_THREAD_LIBS_INIT})
endif()

# link dependencies
if (FMT_ENABLED)
	target_link_libraries(discpp PUBLIC fmt)
endif()

if (USE_RAPID)
	target_include_directories(discpp PUBLIC ${RAPIDJSON_INCLUDE_DIRS})
elseif(USE_SIMD)
	target_link_libraries(discpp PUBLIC simdjson)
endif()


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