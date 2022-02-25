#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct instruction_s instruction_t;

struct instruction_s {
    uint16_t op_code;
    uint8_t u;
    uint16_t nnn;
    uint8_t x;
    uint8_t y;
    uint8_t n;
    uint8_t kk;
};

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

static void decode_instruction(const uint8_t *buf, size_t pc, instruction_t *instruction)
{
    // http://devernay.free.fr/hacks/chip8/C8TECH10.HTM

    // All instructions are 2 bytes long and are stored most-significant-byte first.
    uint16_t op_code = (uint16_t)buf[pc] << 8 | (uint16_t)buf[pc + 1];

    // u - A 4-bit value, the highest 4 bits of the instruction
    uint8_t u = (uint8_t)(op_code >> 12);

    // nnn or addr - A 12-bit value, the lowest 12 bits of the instruction
    uint16_t nnn = op_code << 4;

    // n or nibble - A 4-bit value, the lowest 4 bits of the instruction
    uint8_t n = op_code & 0xf;

    // x - A 4-bit value, the lower 4 bits of the high byte of the instruction
    uint8_t x = (op_code >> 8) & 0xf;

    // y - A 4-bit value, the upper 4 bits of the low byte of the instruction
    uint8_t y = (op_code >> 4) & 0xf;

    // kk or byte - An 8-bit value, the lowest 8 bits of the instruction
    uint8_t kk = op_code & 0xff;

    nnn >>= 4;

    instruction->op_code = op_code;
    instruction->u = u;
    instruction->nnn = nnn;
    instruction->n = n;
    instruction->x = x;
    instruction->y = y;
    instruction->kk = kk;
}

void disas(const uint8_t *buf, size_t pc, bool debug)
{
    char *instruction_str = "UNKNOWN";
    char args[32] = {'\0'};

    instruction_t ins;
    decode_instruction(buf, pc, &ins);

    switch (ins.u) {
        case 0x0:

            switch (ins.kk) {

                // CLEAR - Clear the display
                case 0xe0:
                    instruction_str = "CLEAR";
                    break;

                // RET - Return from a subroutine
                case 0xee:
                    instruction_str = "RET";
                    break;
                default:
                    break;
            }
            break;

        // JMP nnn - Jump to location nnn
        case 0x1:
            instruction_str = "JMP";
            sprintf(args, "%03x", ins.nnn);
            break;

        // CALL nnn - Call subroutine at nnn
        case 0x2:
            instruction_str = "CALL";
            sprintf(args, "%03x", ins.nnn);
            break;

        // SKIP x, kk - Skip next instruction if Vx = kk
        case 0x3:
            instruction_str = "SKIP";
            sprintf(args, "v%01x, %02x", ins.x, ins.kk);
            break;

        // SKIPN x, kk - Skip next instruction if Vx != kk
        case 0x4:
            instruction_str = "SKIPN";
            sprintf(args, "v%01x, %02x", ins.x, ins.kk);
            break;

        // SKIP x, y - Skip next instruction if Vx = Vy
        case 0x5:
            instruction_str = "SKIP";
            sprintf(args, "v%01x, v%01x", ins.x, ins.y);
            break;

        // MVI x, kk - Set Vx = kk
        case 0x6:
            instruction_str = "MVI";
            sprintf(args, "v%01x, %02x", ins.x, ins.kk);
            break;

        // ADD x, kk - Set Vx = Vx + kk
        case 0x7:
            instruction_str = "ADD";
            sprintf(args, "v%01x, %02x", ins.x, ins.kk);
            break;

        case 0x8:
            switch (ins.n) {

                // MOV x, y - Set Vx = Vy
                case 0x0:
                    instruction_str = "MOV";
                    sprintf(args, "v%01x, v%01x", ins.x, ins.y);
                    break;

                // OR x, y - Set Vx = Vx OR Vy
                case 0x1:
                    instruction_str = "OR";
                    sprintf(args, "v%01x, v%01x", ins.x, ins.y);
                    break;


                // AND x, y - Set Vx = Vx AND Vy
                case 0x2:
                    instruction_str = "AND";
                    sprintf(args, "v%01x, v%01x", ins.x, ins.y);
                    break;

                // XOR x, y - Set Vx = Vx XOR Vy
                case 0x3:
                    instruction_str = "XOR";
                    sprintf(args, "v%01x, v%01x", ins.x, ins.y);
                    break;

                // ADD x, y - Set Vx = Vx + Vy, set Vf = carry
                case 0x4:
                    instruction_str = "ADD";
                    sprintf(args, "v%01x, v%01x", ins.x, ins.y);
                    break;

                // SUB x, y - Set Vx = Vx - Vy, set VF = not borrow
                case 0x5:
                    instruction_str = "SUB";
                    sprintf(args, "v%01x, v%01x", ins.x, ins.y);
                    break;

                // SHR x, y - Set Vx = Vx SHR 1
                case 0x6:
                    instruction_str = "SHR";
                    sprintf(args, "v%01x, v%01x", ins.x, ins.y);
                    break;

                // SUBN x, y - Set Vx = Vy - Vx, set VF = NOT borrow
                case 0x7:
                    instruction_str = "SUBN";
                    sprintf(args, "v%01x, v%01x", ins.x, ins.y);
                    break;

                // SHL - Set Vx = Vx SHL 1
                case 0xe:
                    instruction_str = "SHL";
                    sprintf(args, "v%01x, v%01x", ins.x, ins.y);
                    break;
                default:
                    break;
            }
            break;

        // SKIPN x, y - Skip next instruction if Vx != Vy
        case 0x9:
            instruction_str = "SKIPN";
            sprintf(args, "v%01x, v%01x", ins.x, ins.y);
            break;

        // MVI I, nnn - Set I = nnn
        case 0xa:
            instruction_str = "MVI";
            sprintf(args, "I, %03x", ins.nnn);
            break;

        // JMP V0, nnn - Jump to location nnn + V0
        case 0xb:
            instruction_str = "JMP";
            sprintf(args, "V0, %03x", ins.nnn);
            break;

        // RAND x, kk - Set Vx = random byte AND kk
        case 0xc:
            instruction_str = "RAND";
            sprintf(args, "v%01x, %02x", ins.x, ins.kk);
            break;

        // DISP x, y, n - Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision
        case 0xd:
            instruction_str = "DISP";
            sprintf(args, "v%01x, v%01x, %01x", ins.x, ins.y, ins.n);
            break;
        case 0xe:
            switch (ins.kk) {

                // SKIPKEY x - Skip next instruction if key with the value of Vx is pressed
                case 0x9e:
                    instruction_str = "SKIPKEY";
                    sprintf(args, "v%01x", ins.x);
                    break;

                // SKIPNKEY x - Skip next instruction if key with the value of Vx is not pressed
                case 0xA1:
                    instruction_str = "SKIPNKEY";
                    sprintf(args, "v%01x", ins.x);
                    break;
                default:
                    break;
            }
            break;
        case 0xf:
            switch (ins.kk) {

                // MOV x, DELAY - Set Vx = delay timer value
                case 0x07:
                    instruction_str = "MOV";
                    sprintf(args, "v%01x, DELAY", ins.x);
                    break;

                // MOV X, KEY - Wait for a key press, store the value of key in Vx
                case 0x0a:
                    instruction_str = "MOV";
                    sprintf(args, "v%01x, KEY", ins.x);
                    break;

                // MOV DELAY, Vx - Set delay timer = Vx
                case 0x15:
                    instruction_str = "MOV";
                    sprintf(args, "DELAY, v%01x", ins.x);
                    break;

                // MOV SOUND, Vx - Set sound timer = Vx
                case 0x18:
                    instruction_str = "MOV";
                    sprintf(args, "SOUND, v%01x", ins.x);
                    break;

                // ADD I, x - Set I = I + Vx
                case 0x1e:
                    instruction_str = "ADD";
                    sprintf(args, "I, v%01x", ins.x);
                    break;

                // GET_SPRITE_POS - Set I = location of sprite for digit Vx
                case 0x29:
                    instruction_str = "GET_SPRITE_POS";
                    sprintf(args, "I, v%01x", ins.x);
                    break;

                // MOVEBCD x - Store BCD representation of Vx in memory locations I, I+1, and I+2
                case 0x33:
                    instruction_str = "MOVBCD";
                    sprintf(args, "(I), v%01x", ins.x);
                    break;

                // MOVM I, x - Store registers V0 through Vx in memory starting at location I
                case 0x55:
                    instruction_str = "MOVM";
                    sprintf(args, "I, v%01x", ins.x);
                    break;

                // MOVM x, I - Read registers V0 through Vx from memory starting at location I
                case 0x65:
                    instruction_str = "MOVM";
                    sprintf(args, "v%01x, I", ins.x);
                    break;
                default:
                    break;
            }
            break;

        default:
            break;
    }

    printf("%04zx %04x %s %s\n", pc, ins.op_code, instruction_str, args);

    if (debug)
        print_debug(ins);
}