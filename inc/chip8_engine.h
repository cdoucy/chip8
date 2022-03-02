#pragma once

#include <stdint.h>

#define CHIP8_WINDOW_WIDTH    64
#define CHIP8_WINDOW_HEIGHT   32

#define MEMORY_SIZE                 4096
#define V_REGISTERS_SIZE            16
#define STACK_SIZE                  16
#define INITIAL_PROGRAM_COUNTER     0x200
#define MAX_PROG_SIZE               (MEMORY_SIZE - INITIAL_PROGRAM_COUNTER)

typedef struct chip8_engine_s chip8_engine_t;

struct chip8_engine_s {
    // 4KB RAM
    uint8_t memory[MEMORY_SIZE];
    // 8 bits V registers
    uint8_t v[V_REGISTERS_SIZE];
    // 16 bits I register
    uint16_t i;
    // 8 bits delay register timer
    uint8_t delay;
    // 8 bits sound register timer
    uint8_t sound;
    // Stack is an array of 16 16 bits values
    uint16_t stack[STACK_SIZE];
    // 8 bits stack pointer
    uint8_t sp;
    // 16 bits program counter
    uint16_t pc;
    // Size of loaded program
    uint16_t prog_size;
    // 64 * 32 monochrome pixels, stored in bits
    uint64_t screen[CHIP8_WINDOW_HEIGHT];
};

void init_chip8_engine(chip8_engine_t *engine);
void update_chip8_engine(chip8_engine_t *e);
void run_chip8_engine(chip8_engine_t *engine);