cmake_minimum_required(VERSION 3.20)

project(aoc_day_2)

set(CMAKE_CXX_STANDARD 20)

add_compile_options(-Wall -Wextra -Werror -Wpedantic)
add_executable(day2_pt1 main.cpp)

add_executable(day2_pt2 main2.cpp)

#find_package(GTest)
#find_package(absl)
find_package(re2)
find_package(fmt)

target_link_libraries(day2_pt1 re2::re2)
target_link_libraries(day2_pt2 re2::re2 fmt::fmt)

