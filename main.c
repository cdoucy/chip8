#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdbool.h>

#define INITIAL_PROG_COUNTER (0x200)
#define COMMANDS_SIZE (2)

typedef enum command {
    DISAS,
    INTERPRET,
    UNKNOWN
} command_t;

void disas(const uint8_t *buf, size_t pc, bool debug);

static const char *commands[COMMANDS_SIZE + 1] = {
        "disas",
        "interpret",
        NULL
};

static char *read_file_buffaddr(const char *filepath, int bufaddr, size_t *progsize)
{
    struct stat statbuf;
    int fd;
    char *buf;

    if (stat(filepath, &statbuf) != 0) {
        dprintf(2, "stat syscall failed : %s\n", strerror(errno));
        return NULL;
    }

    fd = open(filepath, O_RDONLY);
    if (fd == -1) {
        dprintf(2, "open syscall failed : %s\n", strerror(errno));
        return NULL;
    }

    buf = malloc(sizeof(char) * (statbuf.st_size + bufaddr));
    if (!buf) {
        dprintf(2, "malloc failed");
        return NULL;
    }

    if (read(fd, buf + bufaddr, statbuf.st_size) == -1) {
        free(buf);
        dprintf(2, "read syscall failed : %s\n", strerror(errno));
        return NULL;
    }

    close(fd);

    if (progsize)
        *progsize = statbuf.st_size + bufaddr;

    return buf;
}

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
    size_t pc = INITIAL_PROG_COUNTER;

    if (!(buf = (uint8_t *)read_file_buffaddr(*av, INITIAL_PROG_COUNTER, &progsize)))
        return 1;

    while (pc < progsize) {
        disas(buf, pc, debug);
        pc += 2;
    }

    free(buf);

    return 0;
}

static int interpret(int ac, const char **av)
{
    (void)ac;
    (void)av;

    printf("Interpreter is in development...\n");
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