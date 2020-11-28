cmake_minimum_required(VERSION 3.12)

project(ADVENT_OF_CODE_2020)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory("resources")
add_subdirectory("lib")

include_directories("${CMAKE_CURRENT_BINARY_DIR}")

add_executable(advent2020)
target_sources(advent2020 PRIVATE src/main.cpp)
target_link_libraries(advent2020 catch2)

include_directories("include")
