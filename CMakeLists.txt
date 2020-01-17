cmake_minimum_required(VERSION 2.8.4)
project(CHIP_8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11")

set(SOURCE_FILES main.cpp chip8.cpp disassembler.cpp)
add_executable(chip8 ${SOURCE_FILES})

INCLUDE(FindPkgConfig)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

include_directories(${SDL2_INCLUDE_DIR}
                    ${SDL2_TTF_INCLUDE_DIR})
target_link_libraries(chip8 ${SDL2_LIBRARY}
                            ${SDL2_TTF_LIBRARIES})
