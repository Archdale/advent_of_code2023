cmake_minimum_required(VERSION 3.20)

project(aoc_day_4)

set(CMAKE_CXX_STANDARD 20)

add_compile_options(-Wall -Wextra -Werror)

add_executable(day4_pt1 main.cpp)

add_executable(day4_pt2 main2.cpp)

find_package(re2)
find_package(fmt)

target_link_libraries(day4_pt1 fmt::fmt re2::re2)
target_link_libraries(day4_pt2 fmt::fmt re2::re2)

