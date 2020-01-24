# CHIP-8
A  CHIP-8 emulator in C++, with a debug interface made with SDL

## Compiling and running
Requires cmake, SDL2 and SDL2_ttf

To compile:
```
$ mkdir build
$ cd build
$ cmake ..
$ make
```

To run:
```
./chip8 <binary file>
```

Ensure that ../assets/pixelmix.ttf exists

## Resources and references
- Font from https://www.dafont.com/pixelmix.font
- ROMs combiled by Revival Studios (Information in roms folder README)
- Some of Readme and CMakeLists.txt from https://github.com/JamesGriffin/CHIP-8-Emulator
- Architecture documentation from https://github.com/trapexit/chip-8_documentation
- And also from https://en.wikipedia.org/wiki/CHIP-8#Virtual_machine_description
- Reference and some ops from http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/
