cmake_minimum_required(VERSION 3.12)

project(ADVENT_OF_CODE_2020)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ...::: SETUP LIBS & INCLUDES :::...

include(FetchContent)

FetchContent_Declare(
    catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v2.13.3
)
FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 7.1.3
)
FetchContent_Declare(
    scnlib
    GIT_REPOSITORY https://github.com/eliaskosunen/scnlib.git
    GIT_TAG v0.4
    SOURCE_DIR scn
)

FetchContent_MakeAvailable(catch2 fmt scnlib)

include_directories("${CMAKE_CACHEFILE_DIR}/scn/include" "${CMAKE_CACHEFILE_DIR}/_deps/catch2-src/include")

add_executable(advent2020)
target_sources(advent2020 PRIVATE src/main.cpp)
find_package(Catch2 REQUIRED)
target_link_libraries(advent2020 scn fmt Catch2::Catch2)

#include_directories("include")

#add_subdirectory("lib/tytools")    # libhs + libty



