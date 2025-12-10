#include <SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include "chip8.h" 

const uint8_t keymap[16] = {
    SDL_SCANCODE_X, SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3,
    SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_A,
    SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_Z, SDL_SCANCODE_C,
    SDL_SCANCODE_4, SDL_SCANCODE_R, SDL_SCANCODE_F, SDL_SCANCODE_V
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Uso: %s <rom_file>\n", argv[0]);
        return 1;
    }

    chip8_t cpu;
    chip8_init(&cpu);
    if (!chip8_load_rom(&cpu, argv[1])) {
        return 1;
    }

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        printf("Error SDL: %s\n", SDL_GetError());
        return 1;
    }

    int w = 64;
    int h = 32;
    int scale = 15; // Zoom x15

    SDL_Window* window = SDL_CreateWindow("CHIP-8 Emulator", 
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                          w * scale, h * scale, 0);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    SDL_Texture* texture = SDL_CreateTexture(renderer, 
                                             SDL_PIXELFORMAT_RGBA8888, 
                                             SDL_TEXTUREACCESS_STREAMING, 
                                             w, h);

    uint32_t pixels[2048]; // 64 * 32

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            
            if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
                for (int i = 0; i < 16; i++) {
                    if (event.key.keysym.scancode == keymap[i]) {
                        cpu.keypad[i] = (event.type == SDL_KEYDOWN) ? 1 : 0;
                    }
                }
            }
        }

        for (int i = 0; i < 30; i++) {
            chip8_fetch_decode_exec(&cpu);
        }

        if (cpu.delay > 0) cpu.delay--;
        if (cpu.sound > 0) {
            cpu.sound--;
            printf("\a"); 
        }

        if (cpu.should_draw) {
            for (int i = 0; i < 2048; i++) {
                uint8_t pixel = cpu.pixels[i];
                pixels[i] = (pixel == 1) ? 0x00FF00FF : 0x000000FF; 
            }

            SDL_UpdateTexture(texture, NULL, pixels, 64 * sizeof(uint32_t));
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
            cpu.should_draw = 0;
        }

        SDL_Delay(16);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}