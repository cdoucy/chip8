#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "op_codes.h"

void disas(const uint8_t *buf, uint16_t pc, bool debug);
void print_instruction(uint16_t pc, const instruction_t *i);