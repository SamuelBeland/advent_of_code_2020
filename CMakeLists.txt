cmake_minimum_required(VERSION 3.12)

project(ADVENT_OF_CODE_2020)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_BUILD_TYPE MATCHES Release)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
endif()

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)

add_subdirectory("resources")
add_subdirectory("lib")

include_directories("${CMAKE_CURRENT_BINARY_DIR}" "include")

add_library(adventlib STATIC)
target_sources(adventlib PRIVATE "src/utils.hpp" "src/utils.cpp" "src/day_1.cpp" "src/day_2.cpp")

add_executable(main)
target_sources(main PRIVATE "src/main.cpp")
target_link_libraries(main adventlib)

add_executable(tests)
target_sources(tests PRIVATE "tests/main.cpp")
target_link_libraries(tests adventlib catch2)
