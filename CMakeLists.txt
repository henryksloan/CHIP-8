cmake_minimum_required(VERSION 2.8.4)
project(CHIP_8)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11")

add_subdirectory(src)

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

# set(SOURCE_FILES main.cpp chip8.cpp disassembler.cpp)

INCLUDE(FindPkgConfig)
