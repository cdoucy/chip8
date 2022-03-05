#pragma once

#include <sys/time.h>
#include <stdint.h>
#include <stdbool.h>

// Convert second to microsecond
#define S_TO_US(t) ((t) * 1000000L)
// Convert microsecond to second
#define US_TO_S(t) ((t) / 1000000L)
// Convert microsecond to millisecond
#define US_TO_MS(t) ((t) / 1000L)

typedef struct timeval chip8_clock_t;

void reset_clock(chip8_clock_t *clock);
long int get_elapsed(const chip8_clock_t *clock);
