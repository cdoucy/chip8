#include <string.h>

#include "chip8_engine.h"
#include "op_codes.h"
#include "instructions_executors.h"

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
}

void run_chip8_engine(chip8_engine_t *e)
{
    instruction_t i;
    uint16_t prog_end = e->prog_size + e->pc;

    for (; e->pc < prog_end && e->pc < MEMORY_SIZE; e->pc += 2) {
        read_next_instruction(e->memory, e->pc, &i);
        instructions_executors[i.op_code](e, &i);

    }
}