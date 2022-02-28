#include <stdio.h>

#include "chip8_engine.h"
#include "op_codes.h"

void exec_clear(chip8_engine_t *engine, const instruction_t *i)
{
    printf("%s\n", op_codes_strings[i->op_code]);
}

void exec_ret(chip8_engine_t *engine, const instruction_t *i)
{
    printf("%s\n", op_codes_strings[i->op_code]);
}

void exec_jmp_nnn(chip8_engine_t *engine, const instruction_t *i)
{
    printf("%s\n", op_codes_strings[i->op_code]);
}

void exec_call(chip8_engine_t *engine, const instruction_t *i)
{
    printf("%s\n", op_codes_strings[i->op_code]);
}

void exec_skip_x_kk(chip8_engine_t *engine, const instruction_t *i)
{
    printf("%s\n", op_codes_strings[i->op_code]);
}

void exec_skipn_x_kk(chip8_engine_t *engine, const instruction_t *i)
{
    printf("%s\n", op_codes_strings[i->op_code]);
}

void exec_skip_x_y(chip8_engine_t *engine, const instruction_t *i)
{
    printf("%s\n", op_codes_strings[i->op_code]);
}

void exec_mvi_x_kk(chip8_engine_t *engine, const instruction_t *i)
{
    printf("%s\n", op_codes_strings[i->op_code]);
}

void exec_add_x_kk(chip8_engine_t *engine, const instruction_t *i)
{
    printf("%s\n", op_codes_strings[i->op_code]);
}

void exec_mov_x_y(chip8_engine_t *engine, const instruction_t *i)
{
    printf("%s\n", op_codes_strings[i->op_code]);
}

void exec_or(chip8_engine_t *engine, const instruction_t *i)
{
    printf("%s\n", op_codes_strings[i->op_code]);
}

void exec_and(chip8_engine_t *engine, const instruction_t *i)
{
    printf("%s\n", op_codes_strings[i->op_code]);
}

void exec_xor(chip8_engine_t *engine, const instruction_t *i)
{
    printf("%s\n", op_codes_strings[i->op_code]);
}

void exec_add_x_y(chip8_engine_t *engine, const instruction_t *i)
{
    printf("%s\n", op_codes_strings[i->op_code]);
}

void exec_sub(chip8_engine_t *engine, const instruction_t *i)
{
    printf("%s\n", op_codes_strings[i->op_code]);
}

void exec_shr(chip8_engine_t *engine, const instruction_t *i)
{
    printf("%s\n", op_codes_strings[i->op_code]);
}

void exec_subn(chip8_engine_t *engine, const instruction_t *i)
{
    printf("%s\n", op_codes_strings[i->op_code]);
}

void exec_shl(chip8_engine_t *engine, const instruction_t *i)
{
    printf("%s\n", op_codes_strings[i->op_code]);
}

void exec_skipn_x_y(chip8_engine_t *engine, const instruction_t *i)
{
    printf("%s\n", op_codes_strings[i->op_code]);
}

void exec_mvi_i_nnn(chip8_engine_t *engine, const instruction_t *i)
{
    printf("%s\n", op_codes_strings[i->op_code]);
}

void exec_jmp_v0_nnn(chip8_engine_t *engine, const instruction_t *i)
{
    printf("%s\n", op_codes_strings[i->op_code]);
}

void exec_rand(chip8_engine_t *engine, const instruction_t *i)
{
    printf("%s\n", op_codes_strings[i->op_code]);
}

void exec_disp(chip8_engine_t *engine, const instruction_t *i)
{
    printf("%s\n", op_codes_strings[i->op_code]);
}

void exec_skip_key(chip8_engine_t *engine, const instruction_t *i)
{
    printf("%s\n", op_codes_strings[i->op_code]);
}

void exec_skipn_key(chip8_engine_t *engine, const instruction_t *i)
{
    printf("%s\n", op_codes_strings[i->op_code]);
}

void exec_mov_x_delay(chip8_engine_t *engine, const instruction_t *i)
{
    printf("%s\n", op_codes_strings[i->op_code]);
}

void exec_mov_key(chip8_engine_t *engine, const instruction_t *i)
{
    printf("%s\n", op_codes_strings[i->op_code]);
}

void exec_mov_delay_x(chip8_engine_t *engine, const instruction_t *i)
{
    printf("%s\n", op_codes_strings[i->op_code]);
}

void exec_mov_sound(chip8_engine_t *engine, const instruction_t *i)
{
    printf("%s\n", op_codes_strings[i->op_code]);
}

void exec_add_i_x(chip8_engine_t *engine, const instruction_t *i)
{
    printf("%s\n", op_codes_strings[i->op_code]);
}

void exec_sprite_pos(chip8_engine_t *engine, const instruction_t *i)
{
    printf("%s\n", op_codes_strings[i->op_code]);
}

void exec_movbcd(chip8_engine_t *engine, const instruction_t *i)
{
    printf("%s\n", op_codes_strings[i->op_code]);
}

void exec_movm_i_x(chip8_engine_t *engine, const instruction_t *i)
{
    printf("%s\n", op_codes_strings[i->op_code]);
}

void exec_movm_x_i(chip8_engine_t *engine, const instruction_t *i)
{
    printf("%s\n", op_codes_strings[i->op_code]);
}

void exec_unknown(chip8_engine_t *engine, const instruction_t *i)
{
    printf("%s\n", op_codes_strings[i->op_code]);
}
