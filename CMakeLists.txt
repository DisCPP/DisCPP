# CMakeLists.txt
cmake_minimum_required (VERSION 3.6)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake;${CMAKE_MODULE_PATH}")
project(discpp)

option(BUILD_EXAMPLES "Build example bots" OFF)
option(BUILD_TESTS "Build unit tests" OFF)

find_package(RapidJSON CONFIG REQUIRED)
#find_package(nlohmann_json CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(cpr REQUIRED)

if (WIN32)
	find_package(mbedtls REQUIRED)
endif()
find_package(ixwebsocket REQUIRED)

# Add source files
file(GLOB_RECURSE source_list src/*.cpp)
add_library(discpp STATIC ${source_list} ${events_source_list})

target_include_directories(discpp PUBLIC include PRIVATE include/discpp)

target_include_directories(discpp PUBLIC ${RAPIDJSON_INCLUDE_DIRS})
#target_link_libraries(discpp PUBLIC nlohmann_json::nlohmann_json)
target_link_libraries(discpp PUBLIC OpenSSL::SSL OpenSSL::Crypto)
target_link_libraries(discpp PUBLIC cpr)

if (WIN32)
	target_link_libraries(discpp PUBLIC wsock32 ws2_32 shlwapi ${MBEDTLS_LIBRARY})
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)

	if (USE_TLS)
		target_link_libraries(discpp PUBLIC Crypt32)
	endif()
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