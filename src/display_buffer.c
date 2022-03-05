#include <string.h>

#include "chip8_engine.h"

void clear_display_buffer(display_buffer_t buf)
{
    memset(buf, 0, WINDOW_WIDTH * WINDOW_HEIGHT * sizeof(uint8_t));
}

uint8_t get_pixel(display_buffer_t buf, int x, int y)
{
    x *= WINDOW_SCALE;
    y *= WINDOW_SCALE;

    return buf[y * WINDOW_WIDTH + x];
}

void draw_pixel(display_buffer_t buf, int x, int y, uint8_t color)
{
    x *= WINDOW_SCALE;
    y *= WINDOW_SCALE;

    for (int i = 0; i < WINDOW_SCALE; i++)
        for (int j = 0; j < WINDOW_SCALE; j++)
            buf[(i + y) * WINDOW_WIDTH + (j + x)] = color;
}