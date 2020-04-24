cmake_minimum_required (VERSION 3.6)
project(pingbot_example)

add_executable(pingbot_example main.cpp)
target_include_directories(pingbot_example PUBLIC include)
file(GLOB_RECURSE source_list *.cpp)
target_sources(pingbot_example PRIVATE ${source_list})
target_link_libraries(pingbot_example PUBLIC discpp)
set_target_properties(pingbot_example PROPERTIES CXX_STANDARD 17 CXX_EXTENSIONS OFF)