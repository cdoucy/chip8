#include <string.h>
#include <stdio.h>

#include "chip8_engine.h"
#include "op_codes.h"
#include "instructions_executors.h"

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
        &exec_clear,
        &exec_ret,
        &exec_jmp_nnn,
        &exec_call,
        &exec_skip_x_kk,
        &exec_skipn_x_kk,
        &exec_skip_x_y,
        &exec_mvi_x_kk,
        &exec_add_x_kk,
        &exec_mov_x_y,
        &exec_or,
        &exec_and,
        &exec_xor,
        &exec_add_x_y,
        &exec_sub,
        &exec_shr,
        &exec_subn,
        &exec_shl,
        &exec_skipn_x_y,
        &exec_mvi_i_nnn,
        &exec_jmp_v0_nnn,
        &exec_rand,
        &exec_disp,
        &exec_skip_key,
        &exec_skipn_key,
        &exec_mov_x_delay,
        &exec_mov_key,
        &exec_mov_delay_x,
        &exec_mov_sound,
        &exec_add_i_x,
        &exec_sprite_pos,
        &exec_movbcd,
        &exec_movm_i_x,
        &exec_movm_x_i,
        &exec_unknown,
};

void init_chip8_engine(chip8_engine_t *engine)
{
    memset(engine, 0, sizeof(chip8_engine_t));
    engine->pc = INITIAL_PROGRAM_COUNTER;
    memcpy(engine->memory, chip8_fontset, FONT_SIZE * sizeof(uint8_t));

    reset(&engine->clock);
}

static void check_counters(chip8_engine_t *e)
{
    if (e->delay)
        e->delay--;

    if (e->sound)
        e->sound--;
    else if (false)
        printf("BEEEEEP!\n");
}

void update_chip8_engine(chip8_engine_t *e)
{
    check_counters(e);

    instruction_t i;

    read_next_instruction(e->memory, e->pc, &i);

    printf("%04x %s\n", e->pc, op_codes_strings[i.op_code]);
//    printf("delay = %d, sound = %d\n", e->delay, e->sound);
//    printf("**************************************\n");

    instructions_executors[i.op_code](e, &i);
}

void run_chip8_engine(chip8_engine_t *e)
{
    uint16_t prog_end = e->prog_size + e->pc;

    for (; e->pc < prog_end && e->pc < MEMORY_SIZE;)
        update_chip8_engine(e);
}