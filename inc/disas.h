#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

void disas(const uint8_t *buf, uint16_t pc, bool debug);