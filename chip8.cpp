#include "chip8.h"

void Chip8::init() {
    srand(time(NULL));

    opcode = 0;
    I = 0;
    pc = 0x200;
    sp = 0;

    memory.fill(0);
    V.fill(0);
    gfx.fill(0);
    stack.fill(0);
    key.fill(0);

    // Load fontset
    for(int i = 0; i < 80; ++i)
        memory[i] = chip8_fontset[i];
}

void Chip8::load_program(std::ifstream &file) {
    char buffer[buffer_size]  = { 0 };
    file.read(buffer, buffer_size);
    for(int i = 0; i < buffer_size; ++i)
        memory[i + 512] = buffer[i];
}

void Chip8::step() {
    opcode = memory[pc] << 8 | memory[pc+1];

    short suff;
    int off;
    switch (opcode & 0xF000) {
        case 0x0000:
            switch (opcode & 0x00FF) {
                case 0xE0:
                    // Clear the screen
                    gfx.fill(0);
                    pc += 2;
                    break;
                case 0xEE:
                    // Return from a subroutine
                    pc = stack[sp-1];
                    sp--;
                    break;
                default:
                    // Could be SYS instruction, but treating as NOP
                    break;
            }
            break;
        case 0x1000:
            // Jump to address NNN
            pc = opcode & 0x0FFF;
            break;
        case 0x2000:
            // Call subroutine at NNN
            stack[sp] = pc + 2;
            sp++;
            pc = opcode & 0x0FFF;
            break;
        case 0x3000:
            // Skip if equal
            pc += (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) ? 4 : 2;
            break;
        case 0x4000:
            // Skip if not equal
            pc += (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) ? 4 : 2;
            break;
        case 0x5000:
            // Skip if registers equal
            pc += (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]) ? 4 : 2;
            break;
        case 0x6000:
            // Set VX to NN
            V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
            pc += 2;
            break;
        case 0x7000:
            // Add NN to VX
            V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
            pc += 2;
            break;
        case 0x8000:
            suff = opcode & 0x000F;
            if (suff == 0) {
                V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
            }
            else if (suff > 0 && suff <= 3) {
                std::function<short(short,short)> fn[] = {
                    std::bit_or<short>(),
                    std::bit_and<short>(),
                    std::bit_xor<short>()
                };
                V[(opcode & 0x0F00) >> 8] = fn[suff-1](V[(opcode & 0x0F00) >> 8], V[(opcode & 0x00F0) >> 4]);
            }
            else if (suff == 4) {
                V[0xF] = (V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8])) ? 1 : 0;
                V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
            }
            else if (suff == 5 || suff == 7) {
                int a = (suff == 5) ? V[(opcode & 0x0F00) >> 8] : V[(opcode & 0x00F0) >> 4];
                int b = (suff == 5) ? V[(opcode & 0x00F0) >> 4] : V[(opcode & 0x0F00) >> 8];
                V[0xF] = (a > b) ? 1 : 0;
                V[(opcode & 0x0F00) >> 8] = a-b;
            }
            else if (suff == 6) {
                V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;
                V[(opcode & 0x0F00) >> 8] >>= 1;
            }
            else if (suff == 0xE) {
                // TODO: Is this right??
                V[0xF] = (V[(opcode & 0x0F00) >> 8] & 0x80) >> 7;
                V[(opcode & 0x0F00) >> 8] <<= 1;
            }
            else {
                // TODO
                std::cout << "Unknown opcode: " << opcode << std::endl;
            }
            pc += 2;
            break;
        case 0x9000:
            // Skip if registers not equal
            pc += (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4]) ? 4 : 2;
            break;
        case 0xA000:
            // Set I to the address NNN
            I = opcode & 0x0FFF;
            pc += 2;
            break;
        case 0xB000:
            // Jump to the address NNN plus V0
            pc = (opcode & 0x0FFF) + V[0];
            break;
        case 0xC000:
            // Set VX to the result of a bitwise and operation on a random number (0 to 255) and NN
            V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF) & (rand() % 256);
            pc += 2;
            break;
        case 0xD000:
            // Draw a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels
            /*V[0xF] = 0;
            for (int row = 0; row < (opcode & 0x000F); row++) {
                for (int col = 0; col < 8; col++) {
                    int temp = gfx[(row+V[(opcode & 0x0F00) >> 8])*64 + col + V[(opcode & 0x00F0) >> 4]];
                    gfx[row*64 + col] = memory[I + 8*row + col];
                    if (memory[I + 8*row + col] != temp) V[0xF] = 1;
                }
            }*/
            {unsigned short x = V[(opcode & 0x0F00) >> 8];
            unsigned short y = V[(opcode & 0x00F0) >> 4];
            unsigned short height = opcode & 0x000F;
            unsigned short pixel;
            
            V[0xF] = 0;
            for (int yline = 0; yline < height; yline++)
            {
                pixel = memory[I + yline];
                for(int xline = 0; xline < 8; xline++)
                {
                if((pixel & (0x80 >> xline)) != 0)
                {
                    if(gfx[(x + xline + ((y + yline) * 64))] == 1)
                    V[0xF] = 1;                                 
                    gfx[x + xline + ((y + yline) * 64)] ^= 1;
                }
                }
            }
            pc += 2;}
            break;
        case 0xE000:
            suff = opcode & 0x00FF;
            if (suff == 0x9E) {
                // Skip if key in VX is pressed
                pc += (key[V[(opcode & 0x0F00) >> 8]] == 1) ? 4 : 2;
            }
            else if (suff == 0xA1) {
                // Skip if key in VX is not pressed
                pc += (key[V[(opcode & 0x0F00) >> 8]] == 0) ? 4 : 2;
            }
            else {
                // TODO
                std::cout << "Unknown opcode: " << opcode << std::endl;
            }
            break;
        case 0xF000:
            switch (opcode & 0x00FF) {
                case 0x0007:
                    // Set VX to the value of the delay timer
                    V[(opcode & 0x0F00) >> 8] = delay_timer;
                    pc += 2;
                    break;
                case 0x000A:
                    // Await a keypress (blocking), and then store it in VX
                    // TODO
                    break;
                case 0x0015:
                    // Set the delay timer to VX
                    delay_timer = V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;
                case 0x0018:
                    // Set the sound timer to VX
                    sound_timer = V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;
                case 0x001E:
                    // Add VX to I
                    V[0xF] = (V[(opcode & 0x0F00) >> 8] > (0xFFF-I)) ? 1 : 0;
                    I += V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;
                case 0x0029:
                    // Set I to the location of the sprite for the character in VX
                    I = V[(opcode & 0x0F00) >> 8] * 5;
                    pc += 2;
                    break;
                case 0x0033:
                    memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
                    memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
                    memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
                    pc += 2;
                    break;
                case 0x0055:
                    // Store V0 to VX (including VX) in memory starting at address I
                    for (int i = 0; i <= ((opcode & 0x0F00) >> 8); i++) {
                        memory[I + i] = V[i];
                    }
                    pc += 2;
                    break;
                case 0x0065:
                    // Fill V0 to VX (including VX) with values from memory starting at address I
                    for (int i = 0; i <= ((opcode & 0x0F00) >> 8); i++) {
                        V[i] = memory[I + i];
                    }
                    pc += 2;
                    break;
            }
            break;
        default:
            // TODO
            break;
    }

    if (delay_timer > 0) delay_timer--;
    if (sound_timer > 0) {
        // TODO
        if(sound_timer == 1)
            std::cout << "BEEP!\n";
        sound_timer--;
    }
}