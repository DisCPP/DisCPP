cmake_minimum_required (VERSION 3.6)
project(serverinfo_example)

add_executable(serverinfo_example main.cpp)
target_include_directories(serverinfo_example PUBLIC include)
file(GLOB_RECURSE source_list *.cpp)
target_sources(serverinfo_example PRIVATE ${source_list})
target_link_libraries(serverinfo_example PUBLIC discpp)
set_target_properties(serverinfo_example PROPERTIES CXX_STANDARD 17 CXX_EXTENSIONS OFF)