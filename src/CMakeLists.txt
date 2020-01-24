find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

include_directories(${SDL2_INCLUDE_DIR}
                    ${SDL2_TTF_INCLUDE_DIR})

set(SOURCE_FILES main.cpp chip8.cpp disassembler.cpp)
add_executable(chip8 ${SOURCE_FILES})

target_link_libraries(chip8 ${SDL2_LIBRARY}
                            ${SDL2_TTF_LIBRARIES})
