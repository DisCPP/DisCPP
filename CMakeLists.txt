#CMakeLists.txt
cmake_minimum_required (VERSION 3.6)
project(discpp)

find_package(nlohmann_json CONFIG REQUIRED)
find_package(cpr REQUIRED)
find_package(cpprestsdk CONFIG REQUIRED NAMES)
find_package(Boost COMPONENTS system date_time serialization REQUIRED)

# Add source files
file(GLOB_RECURSE source_list src/*.cpp)
add_library(discpp STATIC ${source_list} ${events_source_list})

target_include_directories(discpp PUBLIC include PRIVATE include/discpp)

target_link_libraries(discpp PRIVATE cpr)
target_link_libraries(discpp PUBLIC nlohmann_json::nlohmann_json)
target_link_libraries(discpp PUBLIC Boost::date_time)
target_link_libraries(discpp PUBLIC Boost::serialization)
target_link_libraries(discpp PRIVATE cpprestsdk::cpprest cpprestsdk::cpprestsdk_zlib_internal cpprestsdk::cpprestsdk_boost_internal)

set_target_properties(discpp PROPERTIES CXX_STANDARD 17 CXX_EXTENSIONS OFF)

add_subdirectory(examples/pingbot)
add_subdirectory(examples/serverinfo)
