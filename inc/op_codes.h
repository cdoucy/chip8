#pragma once

#include <stdint.h>

typedef enum op_code_e op_code_t;
typedef struct instruction_s instruction_t;

#define OP_CODES_SIZE 35

enum op_code_e {
    // CLEAR - Clear the display
    CLEAR,
    // Index 1
    // RET - Return from a subroutine
    RET,
    // JMP nnn - Jump to location nnn
    JMP_NNN,
    // CALL nnn - Call subroutine at nnn
    CALL,
    // SKIP x, kk - Skip next instruction if Vx = kk
    SKIP_X_KK,
    // SKIPN x, kk - Skip next instruction if Vx != kk
    SKIPN_X_KK,
    // SKIP x, y - Skip next instruction if Vx = Vy
    SKIP_X_Y,
    // MVI x, kk - Set Vx = kk
    MVI_X_KK,
    // ADD x, kk - Set Vx = Vx + kk
    ADD_X_KK,
    // MOV x, y - Set Vx = Vy
    MOV_X_Y,
    // OR x, y - Set Vx = Vx OR Vy
    OR,
    // AND x, y - Set Vx = Vx AND Vy
    AND,
    // XOR x, y - Set Vx = Vx XOR Vy
    XOR,
    // ADD x, y - Set Vx = Vx + Vy, set Vf = carry
    ADD_X_Y,
    // SUB x, y - Set Vx = Vx - Vy, set VF = not borrow
    SUB,
    // SHR x, y - Set Vx = Vx SHR 1
    SHR,
    // SUBN x, y - Set Vx = Vy - Vx, set VF = NOT borrow
    SUBN,
    // SHL - Set Vx = Vx SHL 1
    SHL,
    // SKIPN x, y - Skip next instruction if Vx != Vy
    SKIPN_X_Y,
    // MVI I, nnn - Set I = nnn
    MVI_I_NNN,
    // JMP V0, nnn - Jump to location nnn + V0
    JMP_V0_NNN,
    // RAND x, kk - Set Vx = random byte AND kk
    RAND,
    // DISP x, y, n - Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision
    DISP,
    // SKIP_KEY x - Skip next instruction if key with the value of Vx is pressed
    SKIP_KEY,
    // SKIPN_KEY x - Skip next instruction if key with the value of Vx is not pressed
    SKIPN_KEY,
    // MOV x, DELAY - Set Vx = delay timer value
    MOV_X_DELAY,
    // MOV X, KEY - Wait for a key press, store the value of key in Vx
    MOV_KEY,
    // MOV DELAY, Vx - Set delay timer = Vx
    MOV_DELAY_X,
    // MOV SOUND, Vx - Set sound timer = Vx
    MOV_SOUND,
    // ADD I, x - Set I = I + Vx
    ADD_I_X,
    // GET_SPRITE_POS - Set I = location of sprite for digit Vx
    SPRITE_POS,
    // MOVEBCD x - Store BCD representation of Vx in memory locations I, I+1, and I+2
    MOVBCD,
    // MOVM I, x - Store registers V0 through Vx in memory starting at location I
    MOVM_I_X,
    // MOVM x, I - Read registers V0 through Vx from memory starting at location I
    MOVM_X_I,
    // Unknown instruction
    UNKNOWN
};

struct instruction_s {
    uint16_t instruction;
    uint8_t u;
    uint16_t nnn;
    uint8_t x;
    uint8_t y;
    uint8_t n;
    uint8_t kk;
    op_code_t op_code;
};

void read_next_instruction(const uint8_t *buf, uint16_t pc, instruction_t *i);
extern const char *op_codes_strings[OP_CODES_SIZE + 1];