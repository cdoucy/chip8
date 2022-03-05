#include "clock.h"

void reset_clock(chip8_clock_t *clock)
{
    gettimeofday(clock, 0);
}

long int get_elapsed(const chip8_clock_t *clock)
{
    chip8_clock_t now;

    gettimeofday(&now, 0);
    return S_TO_US(now.tv_sec - clock->tv_sec) + (now.tv_usec - clock->tv_usec);
}