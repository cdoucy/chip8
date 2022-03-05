#include <stddef.h>

#include "op_codes.h"

const char *op_codes_strings[OP_CODES_SIZE + 1] = {
        "CLEAR",
        "RET",
        "JMP",
        "CALL",
        "SKIP",
        "SKIPN",
        "SKIP",
        "MVI",
        "ADD",
        "MOV",
        "OR",
        "AND",
        "XOR",
        "ADD",
        "SUB",
        "SHR",
        "SUBN",
        "SHL",
        "SKIPN",
        "MVI",
        "JMP",
        "RAND",
        "DISP",
        "SKIP_KEY",
        "SKIPN_KEY",
        "MOV",
        "MOV",
        "MOV",
        "MOV",
        "ADD",
        "SPRITE_POS",
        "MOVBCD",
        "MOVM",
        "MOVM",
        "UNKNOWN",
        NULL
};

static void decode_instruction(const uint8_t *buf, size_t pc, instruction_t *i)
{
    // http://devernay.free.fr/hacks/chip8/C8TECH10.HTM

    // All instructions are 2 bytes long and are stored most-significant-byte first.
    i->instruction = (uint16_t)buf[pc] << 8 | (uint16_t)buf[pc + 1];

    // u - A 4-bit value, the highest 4 bits of the instruction
    i->u = (uint8_t)(i->instruction >> 12);

    // nnn or addr - A 12-bit value, the lowest 12 bits of the instruction
    i->nnn = i->instruction & 0x0fff;

    // n or nibble - A 4-bit value, the lowest 4 bits of the instruction
    i->n = i->instruction & 0x000f;

    // x - A 4-bit value, the lower 4 bits of the high byte of the instruction
    i->x = (i->instruction >> 8) & 0x000f;

    // y - A 4-bit value, the upper 4 bits of the low byte of the instruction
    i->y = (i->instruction >> 4) & 0x000f;

    // kk or byte - An 8-bit value, the lowest 8 bits of the instruction
    i->kk = i->instruction & 0x00ff;
}

void read_next_instruction(const uint8_t *buf, uint16_t pc, instruction_t *i)
{
    if (!i)
        return;

    decode_instruction(buf, pc, i);

    i->op_code = UNKNOWN;

    switch (i->u) {
        case 0x0:

            switch (i->kk) {

                case 0xe0:
                    i->op_code = CLEAR;
                    break;

                case 0xee:
                    i->op_code = RET;
                    break;
                default:
                    break;
            }
            break;

        case 0x1:
            i->op_code = JMP_NNN;
            break;

        case 0x2:
            i->op_code = CALL;
            break;

        case 0x3:
            i->op_code = SKIP_X_KK;
            break;

        case 0x4:
            i->op_code = SKIPN_X_KK;
            break;

        case 0x5:
            i->op_code = SKIP_X_Y;
            break;

        case 0x6:
            i->op_code = MVI_X_KK;
            break;

        case 0x7:
            i->op_code = ADD_X_KK;
            break;

        case 0x8:
            switch (i->n) {

                case 0x0:
                    i->op_code = MOV_X_Y;
                    break;

                case 0x1:
                    i->op_code = OR;
                    break;

                case 0x2:
                    i->op_code = AND;
                    break;

                case 0x3:
                    i->op_code = XOR;
                    break;

                case 0x4:
                    i->op_code = ADD_X_Y;
                    break;

                case 0x5:
                    i->op_code = SUB;
                    break;

                case 0x6:
                    i->op_code = SHR;
                    break;

                case 0x7:
                    i->op_code = SUBN;
                    break;

                case 0xe:
                    i->op_code = SHL;
                    break;
                default:
                    break;
            }
            break;

        case 0x9:
            i->op_code = SKIPN_X_Y;
            break;

        case 0xa:
            i->op_code = MVI_I_NNN;
            break;

        case 0xb:
            i->op_code = JMP_V0_NNN;
            break;

        case 0xc:
            i->op_code = RAND;
            break;

        case 0xd:
            i->op_code = DISP;
            break;
        case 0xe:
            switch (i->kk) {

                case 0x9e:
                    i->op_code = SKIP_KEY;
                    break;

                case 0xa1:
                    i->op_code = SKIPN_KEY;
                    break;
                default:
                    break;
            }
            break;
        case 0xf:
            switch (i->kk) {

                case 0x07:
                    i->op_code = MOV_X_DELAY;
                    break;

                case 0x0a:
                    i->op_code = MOV_KEY;
                    break;

                case 0x15:
                    i->op_code = MOV_DELAY_X;
                    break;

                case 0x18:
                    i->op_code = MOV_SOUND;
                    break;

                case 0x1e:
                    i->op_code = ADD_I_X;
                    break;

                case 0x29:
                    i->op_code = SPRITE_POS;
                    break;

                case 0x33:
                    i->op_code = MOVBCD;
                    break;

                case 0x55:
                    i->op_code = MOVM_I_X;
                    break;

                case 0x65:
                    i->op_code = MOVM_X_I;
                    break;
                default:
                    break;
            }
            break;

        default:
            break;
    }
}