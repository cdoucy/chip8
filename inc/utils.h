#pragma once

#include <stddef.h>
#include <stdint.h>

uint8_t *read_file_offset(const char *filepath, int offset, size_t *prog_size, long max_size);
bool load_file_to_memory(const char *filepath, uint8_t memory[], uint16_t *prog_size, long memory_size);