#include <string.h>
#include <stdio.h>

#include "chip8_engine.h"
#include "op_codes.h"
#include "instructions_executors.h"
#include "disas.h"

#define MAX_TICKS_PER_CYCLE ((long int)(1000000L / FREQUENCY))

static const uint8_t chip8_fontset[FONT_SIZE] =
{
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

static void (*const instructions_executors[OP_CODES_SIZE])(chip8_engine_t *, const instruction_t *) = {
        &exec_clear,        // CLEAR
        &exec_ret,          // RET
        &exec_jmp_nnn,      // JMP_NNN
        &exec_call,         // CALL
        &exec_skip_x_kk,    // SKIP_X_KK
        &exec_skipn_x_kk,   // SKIPN_X_KK
        &exec_skip_x_y,     // SKIP_X_Y
        &exec_mvi_x_kk,     // MVI_X_KK
        &exec_add_x_kk,     // ADD_X_KK
        &exec_mov_x_y,      // MOV_X_Y
        &exec_or,           // OR
        &exec_and,          // AND
        &exec_xor,          // XOR
        &exec_add_x_y,      // ADD_X_Y
        &exec_sub,          // SUB
        &exec_shr,          // SHR
        &exec_subn,         // SUBN
        &exec_shl,          // SHL
        &exec_skipn_x_y,    // SKIPN_X_Y
        &exec_mvi_i_nnn,    // MVI_I_NNN
        &exec_jmp_v0_nnn,   // JMP_V0_NNN
        &exec_rand,         // RAND
        &exec_disp,         // DISP
        &exec_skip_key,     // SKIP_KEY
        &exec_skipn_key,    // SKIPN_KEY
        &exec_mov_x_delay,  // MOV_X_DELAY
        &exec_mov_key,      // MOV_KEY
        &exec_mov_delay_x,  // MOV_DELAY_X
        &exec_mov_sound,    // MOV_SOUND
        &exec_add_i_x,      // ADD_I_X
        &exec_sprite_pos,   // SPRITE_POS
        &exec_movbcd,       // MOVBCD
        &exec_movm_i_x,     // MOVM_I_X
        &exec_movm_x_i,     // MOVM_X_I
        &exec_unknown,
};

void init_chip8_engine(chip8_engine_t *engine)
{
    memset(engine, 0, sizeof(chip8_engine_t));
    engine->pc = INITIAL_PROGRAM_COUNTER;
    memcpy(engine->memory, chip8_fontset, FONT_SIZE * sizeof(uint8_t));

    reset_clock(&engine->clock);
}

void chip8_check_counters(chip8_engine_t *e)
{
    if (get_elapsed(&e->clock) < MAX_TICKS_PER_CYCLE)
        return;

    reset_clock(&e->clock);

    if (e->delay)
        e->delay--;

    if (e->sound) {
        e->sound--;
        if (!e->sound)
            printf("BEEEEEP!\n");
    }
}

void update_chip8_engine(chip8_engine_t *e, bool disas)
{
    chip8_check_counters(e);

    instruction_t i;

    read_next_instruction(e->memory, e->pc, &i);

    if (disas)
        print_instruction(e->pc, &i);

    instructions_executors[i.op_code](e, &i);
}

void chip8_dump_registers(const chip8_engine_t *e) {
    for (int i = 0; i < 16; i += 4) {
        for (int j = i; j < i + 4; j++) {
            printf("V[%02x] = %02x  ", j, e->v[j]);
        }
        printf("\n");
    }

    printf("I = %04x, sp = %02x, pc = %04x\n", e->i, e->sp, e->pc);
    printf("*********************************************************\n");
}