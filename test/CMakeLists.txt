cmake_minimum_required(VERSION 2.6)
 
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS} ../server ../client ../common)

add_executable(build/run_tests unit_tests.cpp ../server/functions.h ../server/functions.c ../common/common.h ../common/common.c)
target_link_libraries(build/run_tests ${GTEST_LIBRARIES} pthread)
