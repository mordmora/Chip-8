#ifndef CHIP8_H
#define CHIP8_H

#include<stdint.h>
#include<stdbool.h>

#define CHIP8_DISPLAY_WIDTH 64
#define CHIP8_DISPLAY_HEIGHT  32
#define CHIP8_MEMORY_SIZE 4096
#define CHIP8_V_REGISTERS 16
#define CHIP8_STACK_DEPTH 16
#define CHIP8_KEYS 16

#define CHIP8_DISPLAY_WIDTH 64
#define CHIP8_DISPLAY_HEIGHT 32 // Corregido a estándar
#define CHIP8_MEMORY_SIZE 4096
#define CHIP8_V_REGISTERS 16
#define CHIP8_STACK_DEPTH 16
#define CHIP8_KEYS 16

typedef struct chip8_t {
    // bloque grande para optimizazr cache 
    uint8_t mem[CHIP8_MEMORY_SIZE];

    //uint32_t 0xAARRGGBB para SDL
    uint32_t pixels[CHIP8_DISPLAY_WIDTH * CHIP8_DISPLAY_HEIGHT];

    // 16 bits reg
    uint16_t stack[CHIP8_STACK_DEPTH];
    uint16_t I;
    uint16_t PC;

    // 8 bits reg
    uint8_t v[CHIP8_V_REGISTERS];
    
    uint8_t SP;
    uint8_t delay;
    uint8_t sound;

    // I/S
    bool keypad[CHIP8_KEYS];
    bool should_draw;

} chip8_t;

static const uint8_t fontset[80] = {
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
};

void chip8_init(chip8_t*);

bool chip8_load_rom(chip8_t*, const char*);

void chip8_fetch_decode_exec(chip8_t*);

#endif