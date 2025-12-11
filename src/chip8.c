#include "chip8.h"
#include <string.h>
#include <stdio.h>
#include<stdlib.h>


void chip8_init(chip8_t* cpu){

    memset(cpu, 0, sizeof(chip8_t));

    memcpy(&cpu->mem[0x50], fontset, sizeof(fontset));

    cpu->PC = 0x200;

}

bool chip8_load_rom(chip8_t* cpu, const char* fname){

    FILE* rom_f = fopen(fname, "rb");
    if(!rom_f){
        fprintf(stderr, "[ERROR] Rom not found: %s\n", fname);
        return 0;
    }

    fseek(rom_f, 0, SEEK_END);
    long f_size = ftell(rom_f);
    printf("[INFO] ROM size: %ld bytes\n", f_size);

    if(f_size == 0){
        fprintf(stderr, "[INFO] file %s is empty\n", fname);
        fclose(rom_f);
        return 0;
    }

    rewind(rom_f);

    if(f_size > (CHIP8_MEMORY_SIZE - 0x200)){
        fprintf(stderr, "rom size exceeds the memory size");
        fclose(rom_f);
        return 0;
    }

    size_t read_bytes = fread(&cpu->mem[0x200], 1, f_size, rom_f);
    printf("[INFO] Read %zu bytes into memory at 0x200\n", read_bytes);
    
    // Debug: Print first few bytes
    printf("[INFO] First 4 bytes: %02X %02X %02X %02X\n", 
           cpu->mem[0x200], cpu->mem[0x201], cpu->mem[0x202], cpu->mem[0x203]);

    if(ferror(rom_f)){
        fprintf(stderr, "%s is corrupt.\n", fname);
        fclose(rom_f);
        return 0;
    }

    fclose(rom_f);
    return 1;
}

void chip8_fetch_decode_exec(chip8_t *cpu){

    if (cpu->PC >= CHIP8_MEMORY_SIZE - 1) {
        printf("[ERROR] PC out of bounds: %04X\n", cpu->PC);
        return;
    }

    //Fetch
    uint16_t opcode = (cpu->mem[cpu->PC] << 8) | cpu->mem[cpu->PC+1];

    //Decode
    uint8_t op = (opcode & 0xF000) >> 12;
    cpu->PC+=2;
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;
    uint8_t n = opcode & 0x000F;
    uint8_t nn = opcode & 0x00FF;
    uint16_t nnn = opcode & 0x0FFF;


    switch(op){
        case 0x0: {
            if(nn == 0xE0){ //cls

                memset(cpu->pixels, 0, sizeof(cpu->pixels));

                cpu->should_draw = 1;

            }else if (nn == 0xEE){ //ret
                if (cpu->SP > 0){
                    cpu->PC = cpu->stack[--cpu->SP];
                }
            }
            break;
        } 

        case 0x1: { //jump nnn
            cpu->PC = nnn;
            break;
        }

        case 0x2: { //call

            cpu->stack[cpu->SP++] = cpu->PC;
            cpu->PC = nnn;

            break;
        }

        case 0x03: //SE Vx, Byte
        {
            if(cpu->v[x] == nn){
                cpu->PC+=2;
            }
            break;
        }

        case 0x04: //SNE Vx, Byte
        {
            if(cpu->v[x] != nn){
                cpu->PC+=2;
            }
            break;
        }

        case 0x05://SE Vx, Vy
        {
            if(cpu->v[x] == cpu->v[y]){
                cpu->PC += 2;
            }
            break;
        }

        case 0x06: //LD Vx, byte
        {
            cpu->v[x] = nn;
            break;
        }

        case 0x07://ADD Vx, Byte
        {
            cpu->v[x] += nn;
            break;
        }

        case 0x08:
        {

            switch(n){
                case 0x0://LD Vx, Vy
                {
                    cpu->v[x] = cpu->v[y];
                    break;
                }

                case 0x01: //OR Vx, Vy
                {
                    cpu->v[x] = cpu->v[x] | cpu->v[y];
                    break;
                }

                case 0x02://AND Vx, Vy
                {
                    cpu->v[x] = cpu->v[x] & cpu->v[y];
                    break;
                }

                case 0x03: //XOR Vx, Vy
                {
                    cpu->v[x] = cpu->v[x] ^ cpu->v[y];
                    break;
                }

                case 0x04: //ADD Vx,Vy
                //suma Vx con Vy y guarda el resultado en Vx. Si el resultado es mayor a 8 bits, se pone 0x1 en v[0xF]
                {
                    uint16_t res = cpu->v[x] + cpu->v[y];
                    cpu->v[0xF] = (res > 255) ? 1 : 0;
                    cpu->v[x] = res;
                    break;
                }

                case 0x05: //SUB Vx, Vy
                //Resta Vx con Vy y guarda el resultado en Vx, en VF se guarda el estado NOT borrow (si no hay prestamo, 1, si lo hay, 0).
                //si Vx > Vy entonces VF = 1.
                {
                    cpu->v[0xF] = (cpu->v[x] >= cpu->v[y]) ? 1 : 0;
                    cpu->v[x] -= cpu->v[y];
                    break;
                }

                case 0x06:
                {
                    //Desplazar hacia la derecha
                    //SHR
                    cpu->v[0xF] = cpu->v[x] & 0x01;
                    cpu->v[x] = cpu->v[x] >> 1;
                    break;
                }

                case 0x07://SUBN
                {
                    cpu->v[0xF] = (cpu->v[y] >= cpu->v[x]);
                    cpu->v[x] = cpu->v[y] - cpu->v[x];
                    break;
                }

                case 0x0E:{//SHL
                    cpu->v[0xF] = (cpu->v[x] & 0x80) >> 7; 

                    cpu->v[x] = cpu->v[x] << 1;
                    break;
                }
            }
            break;
        }

        case 0x09:{
            if(cpu->v[x] != cpu->v[y]){
                cpu->PC+=2;
            }
            break;
        }

        case 0x0A:{ //LD I, nnn
            cpu->I = nnn;
            break;
        }

        case 0x0B: { //JP v0, addr
            cpu->PC = cpu->v[0] + nnn;
            break;
        }

        case 0x0C: {//RND Vx, Byte
            cpu->v[x] = (rand() % 256) & nn;
            break;
        }

        case 0x0D: {//DRW Vx, Vy, n

            cpu->v[0xF] = 0;
            uint8_t xx = (cpu->v[x] % CHIP8_DISPLAY_WIDTH);
            uint8_t yy = (cpu->v[y] % CHIP8_DISPLAY_HEIGHT);

            for(int i = 0; i < n; i++){
                if(yy + i > CHIP8_DISPLAY_HEIGHT) break;

                uint8_t pixel = cpu->mem[cpu->I + i];

                for(int j = 0; j < 8; j++){

                    if(xx + j > CHIP8_DISPLAY_WIDTH) break;
                    uint8_t bit = (pixel >> (7 - j)) & 1;
                    if(bit == 1){//dibuja
                        uint16_t real_idx = ( (yy+i) * CHIP8_DISPLAY_WIDTH) + (xx+j);

                        if(cpu->pixels[real_idx] == 1)//colision
                            cpu->v[0xF] = 1;
                        cpu->pixels[real_idx] ^= 1;
                    }
                }
            }
            cpu->should_draw = 1;
            //display
            break;
        }

        case 0x0E:{
            switch (nn)
            {
            case 0x9E: //SKP Vx
            {
                if(cpu->v[x] < 16){//esta en el limite

                    if(cpu->keypad[cpu->v[x]]){
                        cpu->PC+=2;
                    }

                }
                break;
            }
            
            case 0xA1: {

                if(cpu->v[x] < 16){
                    if(!cpu->keypad[cpu->v[x]]){
                        cpu->PC+=2;
                    }
                }

                break;
            }
            default:
                break;
            }

            break;
        }

        case 0xF: { 

            switch (nn)
            {
            case 0x07:{ //LD Vx, DT

                cpu->v[x] = cpu->delay;
                break;
            }

            case 0x0A: {

                int is_key_pressed = 0;

                for(int i = 0; i < 16; i++){
                    if(cpu->keypad[i]){
                        cpu->v[x] = i;
                        is_key_pressed = 1;
                        break;
                    }
                }
                
                if(!is_key_pressed) cpu->PC-=2;

                break;
            }

            case 0x15: {
                cpu->delay = cpu->v[x];
                break;
            }

            case 0x18: {
                cpu->sound = cpu->v[x];
                break;
            }

            case 0x1E: {
                cpu->I += cpu->v[x];
                break;
            }

            case 0x29: {
                cpu->I = 0x50 + (cpu->v[x] * 5);
                break;
            }

            case 0x33: {

                cpu->mem[cpu->I] = cpu->v[x] / 100;
                cpu->mem[cpu->I+1] = (cpu->v[x] / 10) % 10;
                cpu->mem[cpu->I+2] = cpu->v[x] % 10;

                break;
            }

            case 0x55: {

                for(int i = 0; i <= x; i++){
                    cpu->mem[cpu->I + i] = cpu->v[i];
                }

                break;
            }

            case 0x65:{

                for(int i = 0; i <= x; i++){
                    cpu->v[i] = cpu->mem[cpu->I + i];
                }
                break;
            }
            
            default:
                break;
            }
        }
    }
}