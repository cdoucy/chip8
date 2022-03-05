#include "clock.h"

void reset(chip8_clock_t *clock)
{
    gettimeofday(clock, 0);
}

// Return true if TIME_WAIT is elapsed
bool check(chip8_clock_t *clock)
{
    chip8_clock_t now;

    gettimeofday(&now, 0);

    long int elapsed = (now.tv_sec - clock->tv_sec) * 1000000L + (now.tv_usec - clock->tv_usec);

    if (elapsed < TIME_WAIT)
        return false;

    reset(clock);

    return true;
}