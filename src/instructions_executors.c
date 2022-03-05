#include <stdio.h>
#include <stdbool.h>

#include "chip8_engine.h"
#include "op_codes.h"
#include "utils.h"

static bool is_v_reg_out_of_bound(uint8_t r)
{
    return r >= V_REGISTERS_SIZE;
}

void exec_clear(chip8_engine_t *e, const instruction_t *i)
{
    (void )i;

    e->pc += 2;
    clear_display_buffer(e->screen);
}

/*
 * RET
 * Return from a subroutine.
 *
 * The interpreter sets the program counter to the address at the top of the stack,
 * then subtracts 1 from the stack pointer.
 */
void exec_ret(chip8_engine_t *e, const instruction_t *i)
{
    (void)i;

    e->pc = e->stack[e->sp--];
}

/*
 * JMP nnn
 * Jump to location nnn.
 *
 * The interpreter sets the program counter to nnn.
 */
void exec_jmp_nnn(chip8_engine_t *e, const instruction_t *i)
{
    e->pc = i->nnn;
}

/*
 *
 * CALL nnn
 * Call subroutine at nnn.
 *
 * The interpreter increments the stack pointer,
 * then puts the current PC on the top of the stack.
 * The PC is then set to nnn.
 */
void exec_call(chip8_engine_t *e, const instruction_t *i)
{
    e->stack[++e->sp] = e->pc + 2;
    e->pc = i->nnn;
}

/*
 * SKIP x, kk
 * Skip next instruction if Vx = kk.
 *
 * The interpreter compares register Vx to kk, and if they are equal,
 * increments the program counter by 2.
 */
void exec_skip_x_kk(chip8_engine_t *e, const instruction_t *i)
{
    // Regular increment
    e->pc += 2;

    if (!is_v_reg_out_of_bound(i->x) && e->v[i->x] == i->kk) {
        // Skip increment
        e->pc += 2;
    }
}

/*
 * SKIPN x, kk
 * Skip next instruction if Vx != kk.

 * The interpreter compares register Vx to kk, and if they are not equal,
 * increments the program counter by 2.
 */
void exec_skipn_x_kk(chip8_engine_t *e, const instruction_t *i)
{
    // Regular increment
    e->pc += 2;

    if (!is_v_reg_out_of_bound(i->x) && e->v[i->x] != i->kk) {
        // Skip increment
        e->pc += 2;
    }
}

/*
 * SKIP x, y
 * Skip next instruction if Vx = Vy.

 * The interpreter compares register Vx to register Vy, and if they are equal,
 * increments the program counter by 2.
 */
void exec_skip_x_y(chip8_engine_t *e, const instruction_t *i)
{
    e->pc += 2;

    if (is_v_reg_out_of_bound(i->x) || is_v_reg_out_of_bound(i->y))
        return;

    if (e->v[i->x] == e->v[i->y])
        e->pc += 2;
}

/*
 * MVI x, kk
 * Set Vx = kk.

 * The interpreter puts the value kk into register Vx.
 */
void exec_mvi_x_kk(chip8_engine_t *e, const instruction_t *i)
{
    e->pc += 2;

    if (is_v_reg_out_of_bound(i->x))
        return;

    e->v[i->x] = i->kk;
}

/*
 * ADD x, kk
 * Set Vx = Vx + kk.

 * Adds the value kk to the value of register Vx, then stores the result in Vx.
 */
void exec_add_x_kk(chip8_engine_t *e, const instruction_t *i)
{
    e->pc += 2;

    if (is_v_reg_out_of_bound(i->x))
        return;

    e->v[i->x] += i->kk;
}

/*
 * MOV x, y
 * Set Vx = Vy.

 * Stores the value of register Vy in register Vx.
 */
void exec_mov_x_y(chip8_engine_t *e, const instruction_t *i)
{
    e->pc += 2;

    if (is_v_reg_out_of_bound(i->x) || is_v_reg_out_of_bound(i->y))
        return;

    e->v[i->x] = e->v[i->y];
}

/*
 * OR x, y
 * Set Vx = Vx OR Vy.
 *
 * Performs a bitwise OR on the values of Vx and Vy, then stores the result in Vx.
 * A bitwise OR compares the corresponding bits from two values, and if either bit is 1,
 * then the same bit in the result is also 1.
 * Otherwise, it is 0.
 */
void exec_or(chip8_engine_t *e, const instruction_t *i)
{
    e->pc += 2;

    if (is_v_reg_out_of_bound(i->x) || is_v_reg_out_of_bound(i->y))
        return;

    e->v[i->x] |= e->v[i->y];
}

/*
 * AND x, y
 * Set Vx = Vx AND Vy.

 * Performs a bitwise AND on the values of Vx and Vy, then stores the result in Vx.
 * A bitwise AND compares the corresponding bits from two values, and if both bits are 1,
 * then the same bit in the result is also 1.
 * Otherwise, it is 0.
 */
void exec_and(chip8_engine_t *e, const instruction_t *i)
{
    e->pc += 2;

    if (is_v_reg_out_of_bound(i->x) || is_v_reg_out_of_bound(i->y))
        return;

    e->v[i->x] &= e->v[i->y];
}

/*
 * XOR Vx, Vy
 * Set Vx = Vx XOR Vy.
 *
 * Performs a bitwise exclusive OR on the values of Vx and Vy, then stores the result in Vx.
 * An exclusive OR compares the corresponding bits from two values, and if the bits are not both the same,
 * then the corresponding bit in the result is set to 1.
 * Otherwise, it is 0.
 */
void exec_xor(chip8_engine_t *e, const instruction_t *i)
{
    e->pc += 2;

    if (is_v_reg_out_of_bound(i->x) || is_v_reg_out_of_bound(i->y))
        return;

    e->v[i->x] ^= e->v[i->y];
}

/*
 * ADD x, y
 * Set Vx = Vx + Vy, set VF = carry.

 * The values of Vx and Vy are added together. If the result is greater than 8 bits (i.e., > 255,)
 * VF is set to 1, otherwise 0.
 * Only the lowest 8 bits of the result are kept, and stored in Vx.
 */
void exec_add_x_y(chip8_engine_t *e, const instruction_t *i)
{
    e->pc += 2;

    if (is_v_reg_out_of_bound(i->x) || is_v_reg_out_of_bound(i->y))
        return;

    uint16_t res = (uint16_t)e->v[i->x] + (uint16_t)e->v[i->y];
    e->v[0xf] = res > 255;
    e->v[i->x] = (uint8_t)(res & 0xff);
}

/*
 * SUB Vx, Vy
 * Set Vx = Vx - Vy, set VF = NOT borrow.

  * If Vx > Vy, then VF is set to 1, otherwise 0.
  * Then Vy is subtracted from Vx, and the results stored in Vx.
 */
void exec_sub(chip8_engine_t *e, const instruction_t *i)
{
    e->pc += 2;

    if (is_v_reg_out_of_bound(i->x) || is_v_reg_out_of_bound(i->y))
        return;

    e->v[0xf] = e->v[i->x] > e->v[i->y];
    e->v[i->x] -= e->v[i->y];
}

/*
 * SHR x
 * Set Vx = Vx SHR 1.
 *
 * If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0.
 * Then Vx is divided by 2.
 */
void exec_shr(chip8_engine_t *e, const instruction_t *i)
{
    e->pc += 2;

    if (is_v_reg_out_of_bound(i->x))
        return;

    e->v[0xf] = e->v[i->x] & 1;
    e->v[i->x] /= 2;
}

/*
 * SUBN Vx, Vy
 * Set Vx = Vy - Vx, set VF = NOT borrow.
 *
 * If Vy > Vx, then VF is set to 1, otherwise 0.
 * Then Vx is subtracted from Vy, and the results stored in Vx.
 */
void exec_subn(chip8_engine_t *e, const instruction_t *i)
{
    e->pc += 2;

    if (is_v_reg_out_of_bound(i->x) || is_v_reg_out_of_bound(i->y))
        return;

    e->v[0xf] = e->v[i->y] > e->v[i->x];
    e->v[i->x] = e->v[i->y] - e->v[i->x];
}

/*
 * SHL Vx
 * Set Vx = Vx SHL 1.
 *
 * If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0.
 * Then Vx is multiplied by 2.
 */
void exec_shl(chip8_engine_t *e, const instruction_t *i)
{
    e->pc += 2;

    if (is_v_reg_out_of_bound(i->x))
        return;

    e->v[0xf] = (e->v[i->x] >> 7) & 1;
    e->v[i->x] *= 2;
}

/*
 * SKIP x, y
 * Skip next instruction if Vx != Vy.
 *
 * The values of Vx and Vy are compared, and if they are not equal,
 * the program counter is increased by 2.
 */
void exec_skipn_x_y(chip8_engine_t *e, const instruction_t *i)
{
    e->pc += 2;

    if (is_v_reg_out_of_bound(i->x) || is_v_reg_out_of_bound(i->y))
        return;

    if (e->v[i->x] != e->v[i->y])
        e->pc += 2;
}

/*
 * MVI I, nnn
 * Set I = nnn.
 *
 * The value of register I is set to nnn.
 */
void exec_mvi_i_nnn(chip8_engine_t *e, const instruction_t *i)
{
    e->pc += 2;

    e->i = i->nnn;
}

/*
 * JP V0, nnn
 * Jump to location nnn + V0.

 * The program counter is set to nnn plus the value of V0.
 */
void exec_jmp_v0_nnn(chip8_engine_t *e, const instruction_t *i)
{
    e->pc = i->nnn + (uint16_t)e->v[0];
}

/*
 * RND x, kk
 * Set Vx = random byte AND kk.
 *
 * The interpreter generates a random number from 0 to 255, which is then ANDed with the value kk.
 * The results are stored in Vx. See instruction 8xy2 for more information on AND.
 */
void exec_rand(chip8_engine_t *e, const instruction_t *i)
{
    e->pc += 2;

    if (is_v_reg_out_of_bound(i->x))
        return;

    e->v[i->x] = generate_random_byte() & i->kk;
}

/*
 *
 * DISP x, y, n
 * Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
 *
 * The interpreter reads n bytes from memory, starting at the address stored in I.
 * These bytes are then displayed as sprites on screen at coordinates (Vx, Vy).
 * Sprites are XORed onto the existing screen.
 * If this causes any pixels to be erased, VF is set to 1, otherwise it is set to 0.
 * If the sprite is positioned so part of it is outside the coordinates of the display,
 * it wraps around to the opposite side of the screen.
 * See instruction 8xy3 for more information on XOR, and section 2.4, Display,
 * for more information on the Chip-8 screen and sprites.
 */
void exec_disp(chip8_engine_t *e, const instruction_t *i)
{
    e->pc += 2;
    if (is_v_reg_out_of_bound(i->x) || is_v_reg_out_of_bound(i->y))
        return;

    uint8_t x = e->v[i->x];
    uint8_t y = e->v[i->y];
    e->v[0xf] = 0;

    for (uint8_t j = 0; j < i->n; j++) {
        uint8_t pixels = e->memory[e->i + j];

        for (uint8_t k = 0; k < 8; k++) {
            uint8_t x_incr = (x + (7 - k)) % CHIP8_WINDOW_WIDTH;
            uint8_t y_incr = (y + j) % CHIP8_WINDOW_HEIGHT;
            uint8_t sprite_pixel = pixels >> k & 1;
            uint8_t screen_pixel = get_pixel(e->screen, x_incr, y_incr);

            e->v[0xf] = sprite_pixel && screen_pixel;

            if (sprite_pixel)
                sprite_pixel = 0xff;

            draw_pixel(e->screen, x_incr, y_incr, screen_pixel ^ sprite_pixel);
        }
    }
}

/*
 * SKIP_KEY x
 * Skip next instruction if key with the value of Vx is pressed.

 * Checks the keyboard, and if the key corresponding to the value of Vx is currently in the down position,
 * PC is increased by 2.
 */
void exec_skip_key(chip8_engine_t *e, const instruction_t *i)
{
    e->pc += 2;

    if (is_v_reg_out_of_bound(i->x))
        return;

    int x = e->v[i->x];

    if (x < KEY_SIZE && e->keyboard[x])
        e->pc += 2;
}

/*
 * SKIPN_KEY x
 * Skip next instruction if key with the value of Vx is not pressed.
 *
 * Checks the keyboard, and if the key corresponding to the value of Vx
 * is currently in the up position, PC is increased by 2.
 */
void exec_skipn_key(chip8_engine_t *e, const instruction_t *i)
{
    e->pc += 2;

    if (is_v_reg_out_of_bound(i->x))
        return;

    int x = e->v[i->x];

    if (x < KEY_SIZE && !e->keyboard[x])
        e->pc += 2;
}

/*
 * MOV x, DELAY
 * Set Vx = delay timer value.
 *
 * The value of DT is placed into Vx.
 */
void exec_mov_x_delay(chip8_engine_t *e, const instruction_t *i)
{
    e->pc += 2;

    if (is_v_reg_out_of_bound(i->x))
        return;

    e->v[i->x] = e->delay;
}

/*
 * MOV x, KEY
 * Wait for a key press, store the value of the key in Vx.
 *
 * All execution stops until a key is pressed, then the value of that key is stored in Vx.
 */

void exec_mov_key(chip8_engine_t *e, const instruction_t *i)
{
    if (is_v_reg_out_of_bound(i->x)) {
        e->pc += 2;
        return;
    }

    for (int j = 0; j < KEY_SIZE; j++) {
        if (e->keyboard[j]) {
            e->v[i->x] = j;
            e->pc += 2;
            break;
        }
    }
}

/*
 * MOV DELAY, x
 * Set delay timer = Vx.
 *
 * DT is set equal to the value of Vx.
 */
void exec_mov_delay_x(chip8_engine_t *e, const instruction_t *i)
{
    e->pc += 2;

    if (is_v_reg_out_of_bound(i->x))
        return;

    e->delay = e->v[i->x];
}

/*
 * MOV SOUND, x
 * Set sound timer = Vx.
 *
 * ST is set equal to the value of Vx.
 */
void exec_mov_sound(chip8_engine_t *e, const instruction_t *i)
{
    e->pc += 2;

    if (is_v_reg_out_of_bound(i->x))
        return;

    e->sound = e->v[i->x];
}

/*
 * ADD I, x
 * Set I = I + Vx.
 *
 * The values of I and Vx are added, and the results are stored in I.
 */
void exec_add_i_x(chip8_engine_t *e, const instruction_t *i)
{
    e->pc += 2;

    if (is_v_reg_out_of_bound(i->x))
        return;

    e->i += e->v[i->x];
}

/*
 * SPRITE_POS I, x
 * Set I = location of sprite for digit Vx.

 * The value of I is set to the location for the hexadecimal sprite corresponding to the value of Vx.
 * See section 2.4, Display, for more information on the Chip-8 hexadecimal font.
 */
void exec_sprite_pos(chip8_engine_t *e, const instruction_t *i)
{
    e->pc += 2;

    if (is_v_reg_out_of_bound(i->x))
        return;

    e->i = e->v[i->x] * 5;
}

/*
 * MOVBCD I, xx
 * Store BCD representation of Vx in memory locations I, I+1, and I+2.
 *
 * The interpreter takes the decimal value of Vx,
 * and places the hundreds digit in memory at location in I,
 * the tens digit at location I+1, and the ones digit at location I+2.
 */
void exec_movbcd(chip8_engine_t *e, const instruction_t *i)
{
    e->pc += 2;

    if (is_v_reg_out_of_bound(i->x))
        return;

    uint8_t x = e->v[i->x];

    e->memory[e->i]     = x / 100;
    e->memory[e->i + 1] = x % 100 / 10;
    e->memory[e->i + 2] = x / 10;
}

/*
 *  MOVM I, x
 * Store registers V0 through Vx in memory starting at location I.
 *
 * The interpreter copies the values of registers V0 through Vx into memory,
 * starting at the address in I
 */
void exec_movm_i_x(chip8_engine_t *e, const instruction_t *i)
{
    e->pc += 2;

    if (is_v_reg_out_of_bound(i->x))
        return;

    for (uint8_t j = 0; j <= i->x; j++)
        e->memory[e->i + j] = e->v[j];
}

/*
 * MOVM x, I
 * Read registers V0 through Vx from memory starting at location I.
 *
 * The interpreter reads values from memory starting at location I into registers V0 through Vx.
 */
void exec_movm_x_i(chip8_engine_t *e, const instruction_t *i)
{
    e->pc += 2;

    if (is_v_reg_out_of_bound(i->x))
        return;

    for (uint8_t j = 0; j <= i->x; j++)
        e->v[j] = e->memory[e->i + j];
}

void exec_unknown(chip8_engine_t *e, const instruction_t *i)
{
    (void)i;
    e->pc += 2;
}
