cmake_minimum_required (VERSION 3.6)
project(tests)

find_package(GTest CONFIG REQUIRED)
find_package(cpr REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

add_executable(tests src/main.cpp)
target_include_directories(tests PUBLIC include)
file(GLOB_RECURSE source_list src/*.cpp)
target_sources(tests PRIVATE ${source_list})

include_directories(
    ${GTEST_INCLUDE_DIRS}
)

add_test(MyTests ${PROJECT_NAME})
target_link_libraries(tests PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
target_link_libraries(tests PUBLIC discpp)
target_link_libraries(tests PUBLIC cpr)
target_link_libraries(tests PUBLIC nlohmann_json::nlohmann_json)
set_target_properties(tests PROPERTIES CXX_STANDARD 17 CXX_EXTENSIONS OFF)