cmake_minimum_required(VERSION 3.20)

project(aoc_day_1)

set(CMAKE_CXX_STANDARD 20)

add_executable(day1_pt1 main.cpp)

add_executable(day1_pt2 main2.cpp)

find_package(GTest)
find_package(absl)
find_package(re2)

target_link_libraries(day1_pt2 re2::re2)
