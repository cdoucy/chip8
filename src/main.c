#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "disas.h"
#include "utils.h"
#include "chip8_engine.h"

#define COMMANDS_SIZE (2)

typedef enum command {
    DISAS,
    INTERPRET,
    UNKNOWN
} command_t;

static const char *commands[COMMANDS_SIZE + 1] = {
        "disas",
        "interpret",
        NULL
};

static int usage(const char *prog_name, bool is_error)
{
    int fd = is_error + 1;

    dprintf(fd, \
        "USAGE\n"
        "\t%s disas|interpret file.ch8 [--debug]\n"
    , prog_name);

    return is_error;
}

static int disassemble(int ac, const char **av)
{
    bool debug = ac > 1 && !strcmp(av[1], "--debug");
    size_t progsize = 0;
    uint8_t *buf = NULL;
    uint16_t pc = INITIAL_PROGRAM_COUNTER;

    if (!(buf = read_file_offset(*av, INITIAL_PROGRAM_COUNTER, &progsize, MAX_PROG_SIZE)))
        return 1;


    for (; (size_t)pc < progsize; pc += 2)
        disas(buf, pc , debug);

    free(buf);

    return 0;
}

static int interpret(int ac, const char **av)
{
    chip8_engine_t engine;
    bool error = false;

    init_chip8_engine(&engine);
    error = load_file_to_memory(*av, engine.memory + INITIAL_PROGRAM_COUNTER, &engine.prog_size, MAX_PROG_SIZE);

    if (error)
        return 1;

    run_chip8_engine(&engine);

    return 0;
}

int main(int ac, const char **av)
{
    command_t cmd = UNKNOWN;

    if (ac < 3)
        return usage(*av, true);

    const char *str_cmd = av[1];

    for (int i = 0; commands[i]; i++) {
        if (!strcmp(str_cmd, commands[i])) {
            cmd = i;
            break;
        }
    }

    switch (cmd) {
        case DISAS:
            return disassemble(ac - 2, av + 2);
        case INTERPRET:
            return interpret(ac - 2, av + 2);
        default:
            return usage(*av, true);
    }
}