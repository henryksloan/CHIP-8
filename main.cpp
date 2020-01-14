#include "chip8.h"
#include "disassembler.h"

#include <SDL2/SDL.h> 
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <sstream>
#include <iomanip>

const int WINDOW_WIDTH = 840;
const int WINDOW_HEIGHT = 520;
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 320;
const int FRAMES_PER_SECOND = 60;
const int TEXT_SIZE = 8;
const int GAP_SIZE = 4;

int main(int argc, char **argv) {
    Chip8 chip8;
    chip8.init();

    Disassembler disas;

    chip8.load_program(argv[1]);
    disas.disassemble(argv[1]);
    std::vector<std::string> instr = disas.get_instructions();

    bool quit = false;
    SDL_Event event;
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    TTF_Font *font = TTF_OpenFont("pixelmix.ttf", TEXT_SIZE);
    SDL_Window *window = SDL_CreateWindow("Chip8 Emulator",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Surface *surface;
    SDL_Texture *texture;
    SDL_Rect dstrect;
    std::stringstream ss;
    int texW = 0;
    int texH = 0;
    int prev = chip8.get_pc();

    int start = SDL_GetTicks();
    while (!quit) {
        start = SDL_GetTicks();
        while(SDL_PollEvent(&event) != 0) {
            if(event.type == SDL_QUIT) {
                quit = true;
            }
        }

        Uint8* k = (Uint8*) SDL_GetKeyboardState(NULL);
        chip8.set_key({k[SDL_SCANCODE_1], k[SDL_SCANCODE_2], k[SDL_SCANCODE_3], k[SDL_SCANCODE_4],
                       k[SDL_SCANCODE_Q], k[SDL_SCANCODE_W], k[SDL_SCANCODE_E], k[SDL_SCANCODE_R],
                       k[SDL_SCANCODE_A], k[SDL_SCANCODE_S], k[SDL_SCANCODE_D], k[SDL_SCANCODE_F],
                       k[SDL_SCANCODE_Z], k[SDL_SCANCODE_X], k[SDL_SCANCODE_C], k[SDL_SCANCODE_V]});

        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        std::array<unsigned char, 64*32> gfx = chip8.get_gfx();
        for (int row = 0; row < 32; row++) {
            for (int col = 0; col < 64; col++) {
                if (gfx[row*64+col]) {
                    SDL_Rect fillRect = { col * (SCREEN_WIDTH/64), row * (SCREEN_HEIGHT/32), SCREEN_WIDTH/64, SCREEN_HEIGHT/32 };
                    SDL_RenderFillRect(renderer, &fillRect);
                }
            }
        }

        for (int i = 0; i < WINDOW_WIDTH; i++) {
            SDL_RenderDrawPoint(renderer, i, SCREEN_HEIGHT);
        }
        for (int i = 0; i < WINDOW_HEIGHT; i++) {
            SDL_RenderDrawPoint(renderer, SCREEN_WIDTH, i);
        }

        int n = prev;
        int p = chip8.get_pc();
        if (((p > n) && ((p-n) > 30)) || ((p < n) && ((n-p) > 1))) n = chip8.get_pc();
        // std::cout << n << std::endl;
        int prog_start = (n-512)/2 - 1;
        if (prog_start < 0) prog_start = 0;
        int prog_end = prog_start+32;
        if (prog_end > instr.size()) prog_end = instr.size()-prog_start;
        for (int i = 0; i < prog_end-prog_start; i++) {
            ss = std::stringstream();
            ss << std::hex << std::uppercase << std::setw(4) << std::setfill('0')
               << (prog_start+i)*2 << std::dec << ": " << instr[prog_start+i];
            // if ((chip8.get_pc()-512)/2 == prog_start+i) ss << 'F';
            dstrect = { SCREEN_WIDTH+8, GAP_SIZE*i + 6*i, texW, texH };
            SDL_Color text_color = { 255, 255, 255 };
            if ((chip8.get_pc()-512)/2 == prog_start+i) {
                SDL_RenderFillRect(renderer, &dstrect);
                text_color = {0, 0, 0};
            }
            surface = TTF_RenderText_Solid(font, ss.str().c_str(), text_color);
            texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
            SDL_RenderCopy(renderer, texture, NULL, &dstrect);
            SDL_DestroyTexture(texture);
            SDL_FreeSurface(surface);
        };

        std::array<unsigned char, 16> V = chip8.get_V();
        for (int i = 0; i < 16; i++) {
            ss = std::stringstream();
            ss << "V" << std::hex << std::uppercase << i
                << std::dec << std::nouppercase << ": "
                << std::hex << std::setw(2) << std::setfill('0') << +V[i] << std::dec;
            surface = TTF_RenderText_Solid(font, ss.str().c_str(), { 255, 255, 255 });
            texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
            dstrect = { SCREEN_WIDTH+8, SCREEN_HEIGHT + 8 + GAP_SIZE*i + (8*i), texW, texH };
            SDL_RenderCopy(renderer, texture, NULL, &dstrect);
            SDL_DestroyTexture(texture);
            SDL_FreeSurface(surface);
        };

        ss = std::stringstream();
        ss << "DT: " << std::hex << std::setw(2) << std::setfill('0') << +chip8.get_delay_timer() << std::dec;
        surface = TTF_RenderText_Solid(font, ss.str().c_str(), { 255, 255, 255 });
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
        dstrect = { SCREEN_WIDTH + 100, SCREEN_HEIGHT + 8, texW, texH };
        SDL_RenderCopy(renderer, texture, NULL, &dstrect);
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(surface);

        ss = std::stringstream();
        ss << "ST: " << std::hex << std::setw(2) << std::setfill('0') << +chip8.get_sound_timer() << std::dec;
        surface = TTF_RenderText_Solid(font, ss.str().c_str(), { 255, 255, 255 });
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
        dstrect = { SCREEN_WIDTH + 100, SCREEN_HEIGHT + 8 + GAP_SIZE+8, texW, texH };
        SDL_RenderCopy(renderer, texture, NULL, &dstrect);
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(surface);

        ss = std::stringstream();
        ss << "I: " << std::hex << std::setw(4) << std::setfill('0') << +chip8.get_I() << std::dec;
        surface = TTF_RenderText_Solid(font, ss.str().c_str(), { 255, 255, 255 });
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
        dstrect = { SCREEN_WIDTH + 100, SCREEN_HEIGHT + 8 + GAP_SIZE*8, texW, texH };
        SDL_RenderCopy(renderer, texture, NULL, &dstrect);
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(surface);

        ss = std::stringstream();
        ss << "PC: " << std::hex << std::setw(4) << std::setfill('0') << +chip8.get_pc() << std::dec;
        surface = TTF_RenderText_Solid(font, ss.str().c_str(), { 255, 255, 255 });
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
        dstrect = { SCREEN_WIDTH + 100, SCREEN_HEIGHT + 8 + GAP_SIZE*11, texW, texH };
        SDL_RenderCopy(renderer, texture, NULL, &dstrect);
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(surface);

        ss = std::stringstream();
        ss << "SP: " << std::hex << std::setw(2) << std::setfill('0') << +chip8.get_sp() << std::dec;
        surface = TTF_RenderText_Solid(font, ss.str().c_str(), { 255, 255, 255 });
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
        dstrect = { SCREEN_WIDTH + 100, SCREEN_HEIGHT + 8 + GAP_SIZE*14, texW, texH };
        SDL_RenderCopy(renderer, texture, NULL, &dstrect);
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(surface);

        /* std::array<unsigned short, 16> stack = chip8.get_stack();
        for (int i = 0; i < ((chip8.get_sp() <= 9) ? chip8.get_sp() : 9); i++) {
            ss = std::stringstream();
            ss << std::hex << std::uppercase << i
                << std::dec << std::nouppercase << ": "
                << std::hex << std::setw(2) << std::setfill('0') << +stack[i] << std::dec;
            surface = TTF_RenderText_Solid(font, ss.str().c_str(), { 255, 255, 255 });
            texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
            dstrect = { SCREEN_WIDTH+100, (TEXT_SIZE*8+16) + TEXT_SIZE*i + (8*i), texW, texH };
            SDL_RenderCopy(renderer, texture, NULL, &dstrect);
            SDL_DestroyTexture(texture);
            SDL_FreeSurface(surface);
        }
        if (chip8.get_sp() > 9) {
            surface = TTF_RenderText_Solid(font, "...", { 255, 255, 255 });
            texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
            dstrect = { SCREEN_WIDTH+100, (TEXT_SIZE*8+16) + TEXT_SIZE*10 + (8*10), texW, texH };
            SDL_RenderCopy(renderer, texture, NULL, &dstrect);
            SDL_DestroyTexture(texture);
            SDL_FreeSurface(surface);
        } */

        SDL_RenderPresent(renderer);
        prev = n;
        chip8.step();

        /* int frame_ticks = SDL_GetTicks();
        if((1000/FRAMES_PER_SECOND) > (frame_ticks - start)) {
            SDL_Delay((1000/FRAMES_PER_SECOND) - (frame_ticks - start));
        } */
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    TTF_CloseFont(font);
    return 0;
}