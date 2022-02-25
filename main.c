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

void disas(const uint8_t *buf, size_t pc, bool debug);

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
        dprintf(2, "read syscall failed : %s\n", strerror(errno));
        return NULL;
    }

    close(fd);

    if (progsize)
        *progsize = statbuf.st_size + bufaddr;

    return buf;
}


int main(int ac, const char **av)
{
    if (ac < 2) {
        dprintf(2, "Usage : %s program.ch8\n", *av);
        return 1;
    }

    bool debug = ac > 2 && !strcmp(av[2], "--debug");

    size_t progsize;
    uint8_t *buf = (uint8_t *)read_file_buffaddr(av[1], INITIAL_PROG_COUNTER, &progsize);
    if (!buf)
        return 1;

    size_t pc = INITIAL_PROG_COUNTER;

    while (pc < progsize) {
        disas(buf, pc, debug);
        pc += 2;
    }

    free(buf);

    return 0;
}