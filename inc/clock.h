#pragma once

#include <sys/time.h>
#include <stdint.h>
#include <stdbool.h>

#define FREQUENCY 60
#define TIME_WAIT ((long int)(1000000L / FREQUENCY))

typedef struct timeval chip8_clock_t;

void reset(chip8_clock_t *clock);
bool check(chip8_clock_t *clock);