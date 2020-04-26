cmake_minimum_required (VERSION 3.6)
project(tests)

find_package(GTest CONFIG REQUIRED)
add_executable(tests src/main.cpp)
target_include_directories(tests PUBLIC include/discpp)
file(GLOB_RECURSE source_list src/*.cpp)

target_link_libraries(tests PRIVATE unit_tests)
target_link_libraries(tests PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)

add_test(
  NAME
    unit
  COMMAND
    ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/unit_tests
)