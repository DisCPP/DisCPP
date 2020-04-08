#CMakeLists.txt
cmake_minimum_required (VERSION 3.6)
project(dpp)

find_package(nlohmann_json CONFIG REQUIRED)
find_package(cpr REQUIRED)
find_package(cpprestsdk CONFIG REQUIRED NAMES)
find_package(Boost COMPONENTS system date_time serialization REQUIRED)

# Add source files
file(GLOB_RECURSE source_list src/*.cpp)
add_library(discordpp STATIC ${source_list} ${events_source_list})

target_include_directories(discordpp PUBLIC include PRIVATE include/discordpp)
#target_include_directories(discordpp PRIVATE include/discordpp/events)

target_link_libraries(discordpp PRIVATE cpr)
target_link_libraries(discordpp PUBLIC nlohmann_json::nlohmann_json)
target_link_libraries(discordpp PUBLIC Boost::date_time)
target_link_libraries(discordpp PUBLIC Boost::serialization)
target_link_libraries(discordpp PRIVATE cpprestsdk::cpprest cpprestsdk::cpprestsdk_zlib_internal cpprestsdk::cpprestsdk_boost_internal)

set_target_properties(discordpp PROPERTIES CXX_STANDARD 17 CXX_EXTENSIONS OFF)

add_executable(bot_example examples/basic/main.cpp)
target_sources(bot_example PRIVATE examples/basic/ping_command.cpp)

target_include_directories(bot_example PUBLIC examples/basic/include)

target_link_libraries(bot_example PUBLIC discordpp)

set_target_properties(bot_example PROPERTIES CXX_STANDARD 17 CXX_EXTENSIONS OFF)
