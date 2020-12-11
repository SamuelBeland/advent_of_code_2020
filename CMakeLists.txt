cmake_minimum_required(VERSION 3.12)

project(ADVENT_OF_CODE_2020)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_BUILD_TYPE MATCHES Release)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
endif()

add_subdirectory("lib/catch2")

# This copies the default input text files. 
# It also configures a header file containing the basic function
#  signatures and the paths to the input files.
add_subdirectory("resources")

include_directories("include" "${CMAKE_BINARY_DIR}/resources")

add_library(adventlib STATIC)
target_sources(adventlib PRIVATE
    "src/utils.cpp" "src/utils.hpp"
    "src/day_1.cpp"
    "src/day_2.cpp"
    "src/day_3.cpp"
    "src/day_4.cpp"
    "src/day_5.cpp"
    "src/day_6.cpp"
    "src/day_7.cpp"
    "src/day_8.cpp"
    "src/day_9.cpp"
    "src/day_10.cpp")

add_executable(main)
target_sources(main PRIVATE "src/main.cpp")
target_link_libraries(main adventlib)

add_executable(tests)
target_sources(tests PRIVATE "tests/main.cpp")
target_link_libraries(tests adventlib catch2)
