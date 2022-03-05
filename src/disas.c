#include <stdio.h>
#include <stdbool.h>

#include "op_codes.h"

#define BUF_SIZE 32

static const char bit_to_char[2] = {
        '0',
        '1'
};

static void byte_to_binary(uint8_t v, char buf[9])
{
    for (int i = 0; i < 8; i ++)
        buf[i] = bit_to_char[(v >> i & 1)];

    buf[8] = '\0';
}

static void short_to_binary(uint16_t v, char buf[17])
{
    for (int i = 0; i < 16; i ++)
        buf[i] = bit_to_char[(v >> i & 1)];

    buf[16] = '\0';
}

static void print_debug(instruction_t ins) {
    char byte_buf[9];
    char short_buf[17];

    short_to_binary(ins.op_code, short_buf);
    printf("op_code | hex = [%04x], bin = [%s]\n", ins.op_code, short_buf);

    byte_to_binary(ins.u, byte_buf);
    printf("u       | hex = [%04x], bin = [%s]\n", ins.u, byte_buf);

    short_to_binary(ins.nnn, short_buf);
    printf("nnn     | hex = [%04x], bin = [%s]\n", ins.nnn, short_buf);

    byte_to_binary(ins.x, byte_buf);
    printf("x       | hex = [%04x], bin = [%s]\n", ins.x, byte_buf);

    byte_to_binary(ins.y, byte_buf);
    printf("y       | hex = [%04x], bin = [%s]\n", ins.y, byte_buf);

    byte_to_binary(ins.n, byte_buf);
    printf("n       | hex = [%04x], bin = [%s]\n", ins.n, byte_buf);

    byte_to_binary(ins.kk, byte_buf);
    printf("kk      | hex = [%04x], bin = [%s]\n", ins.kk, byte_buf);

    printf("\n============================================================================\n\n");
}

static void no_param(const instruction_t *i, char buf[BUF_SIZE])
{
    (void)i;
    (void)buf;
}

static void param_nnn(const instruction_t *i, char buf[BUF_SIZE])
{
    sprintf(buf, "%03x", i->nnn);
}

static void params_x_kk(const instruction_t *i, char buf[BUF_SIZE])
{
    sprintf(buf, "v%01x, %02x", i->x, i->kk);
}

static void params_x_y(const instruction_t *i, char buf[BUF_SIZE])
{
    sprintf(buf, "v%01x, v%01x", i->x, i->y);
}

static void params_x_y_n(const instruction_t *i, char buf[BUF_SIZE])
{
    sprintf(buf, "v%01x, v%01x, %01x", i->x, i->y, i->n);
}

static void param_x(const instruction_t *i, char buf[BUF_SIZE])
{
    sprintf(buf, "v%01x", i->x);
}

static void params_i_nnn(const instruction_t *i, char buf[BUF_SIZE])
{
    sprintf(buf, "I, %03x", i->nnn);
}

static void params_v0_nnn(const instruction_t *i, char buf[BUF_SIZE])
{
    sprintf(buf, "v0, %03x", i->nnn);
}

static void params_x_delay(const instruction_t *i, char buf[BUF_SIZE])
{
    sprintf(buf, "v%01x, DELAY", i->x);
}

static void params_x_key(const instruction_t *i, char buf[BUF_SIZE])
{
    sprintf(buf, "v%01x, KEY", i->x);
}

static void params_delay_x(const instruction_t *i, char buf[BUF_SIZE])
{
    sprintf(buf, "DELAY, v%01x", i->x);
}

static void params_sound_x(const instruction_t *i, char buf[BUF_SIZE])
{
    sprintf(buf, "SOUND, v%01x", i->x);
}

static void params_i_x(const instruction_t *i, char buf[BUF_SIZE])
{
    sprintf(buf, "I, v%01x", i->x);
}

static void params_x_i(const instruction_t *i, char buf[BUF_SIZE])
{
    sprintf(buf, "v%01x, I", i->x);
}

static void (*const params_functions[OP_CODES_SIZE])(const instruction_t *, char buf[BUF_SIZE]) = {
    // CLEAR
    &no_param,
    // RET
    &no_param,
    // JMP_NNN
    &param_nnn,
    // CALL,
    &param_nnn,
    // SKIP_X_KK
    &params_x_kk,
    // SKIPN_X_KK
    &params_x_kk,
    // SKIP_X_Y,
    &params_x_y,
    // MVI_X_KK
    &params_x_kk,
    // ADD_X_KK,
    &params_x_kk,
    // MOV_X_Y
    &params_x_y,
    // OR
    &params_x_y,
    // AND
    &params_x_y,
    // XOR,
    &params_x_y,
    // ADD_X_Y
    &params_x_y,
    // SUB,
    &params_x_y,
    // SHR
    &params_x_y,
    // SUBN
    &params_x_y,
    // SHL
    &params_x_y,
    // SKIPN_X_Y
    &params_x_y,
    // MVI_I_NNN
    &params_i_nnn,
    // JMP_V0_NNN
    &params_v0_nnn,
    // RAND
    &params_x_kk,
    // DISP
    &params_x_y_n,
    // SKIP_KEY
    &param_x,
    // SKIPN_KEY
    &param_x,
    // MOV_X_DELAY
    &params_x_delay,
    // MOV_KEY
    &params_x_key,
    // MOV_DELAY_X
    &params_delay_x,
    // MOV_SOUND
    &params_sound_x,
    // ADD_I_X
    &params_i_x,
    // SPRITE_POS
    &params_i_x,
    // MOVBCD
    &params_i_x,
    // MOVM_I_X
    &params_i_x,
    // MOVM_X_I,
    &params_x_i,
    // UNKNOWN
    &no_param
};

void print_instruction(uint16_t pc, const instruction_t *i)
{
    char instruction_params[BUF_SIZE] = {'\0'};

    params_functions[i->op_code](i, instruction_params);

    printf(
            "%04hx %04x %s %s\n",
            pc,
            i->instruction,
            op_codes_strings[i->op_code],
            instruction_params
    );
}

void disas(const uint8_t *buf, uint16_t pc, bool debug)
{
    instruction_t instruction;

    read_next_instruction(buf, pc, &instruction);
    print_instruction(pc, &instruction);

    if (debug)
        print_debug(instruction);
}