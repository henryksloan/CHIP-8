#ifndef CHIP8_H
#define CHIP8_H

#include <array>
#include <functional>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>

const int buffer_size = 4096-512;

class Chip8 {
 public:
    void init();
    void load_program(std::string filename);
    void step();

    void set_key(std::array<unsigned char, 16> key) { this->key = key; }

    std::array<unsigned char, 4096> get_memory() { return memory; }
    std::array<unsigned char, 16> get_V() { return V; }
    std::array<unsigned char, 64*32> get_gfx() { return gfx; }
    std::array<unsigned short, 16> get_stack() { return stack; }

    unsigned short get_I() { return I; }
    unsigned short get_pc() { return pc; }
    unsigned short get_sp() { return sp; }

    unsigned char get_delay_timer() { return delay_timer; }
    unsigned char get_sound_timer() { return sound_timer; }

 private:
    unsigned short opcode;
    unsigned short I;
    unsigned short pc;

    std::array<unsigned char, 4096> memory;
    std::array<unsigned char, 16> V;
    std::array<unsigned char, 64*32> gfx;

    unsigned char delay_timer;
    unsigned char sound_timer;

    std::array<unsigned short, 16> stack;
    unsigned short sp;

    std::array<unsigned char, 16> key;

    const std::array<unsigned char, 80> chip8_fontset =
        { {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
        } };
};

#endif
