#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>
#include <stdbool.h>

uint8_t *read_file_offset(const char *filepath, int offset, size_t *prog_size, long max_size)
{
    struct stat statbuf;
    int fd;
    uint8_t *buf;

    if (stat(filepath, &statbuf) != 0) {
        dprintf(2, "%s : %s\n", filepath, strerror(errno));
        return NULL;
    }

    if (statbuf.st_size > max_size) {
        dprintf(2, "%s : to large file. Memory size is %zu, file size is %zu.\n", filepath, max_size, statbuf.st_size);
        return NULL;
    }

    fd = open(filepath, O_RDONLY);
    if (fd == -1) {
        dprintf(2, "%s : %s\n", filepath, strerror(errno));
        return NULL;
    }

    buf = malloc(sizeof(char) * (statbuf.st_size + offset));
    if (!buf) {
        dprintf(2, "malloc failed");
        return NULL;
    }

    if (read(fd, buf + offset, statbuf.st_size) == -1) {
        free(buf);
        dprintf(2, "%s : %s\n", filepath, strerror(errno));
        return NULL;
    }

    close(fd);

    if (prog_size)
        *prog_size = statbuf.st_size + offset;

    return buf;
}

bool load_file_to_memory(const char *filepath, uint8_t memory[], uint16_t *prog_size, long memory_size)
{
    struct stat statbuf;
    int fd;

    if (stat(filepath, &statbuf) != 0) {
        dprintf(2, "%s : %s\n", filepath, strerror(errno));
        return true;
    }

    if (statbuf.st_size > memory_size) {
        dprintf(2, "%s : to large file. Memory size is %zu, file size is %zu.\n", filepath, memory_size, statbuf.st_size);
        return true;
    }

    fd = open(filepath, O_RDONLY);
    if (fd == -1) {
        dprintf(2, "%s : %s\n", filepath, strerror(errno));
        return true;
    }

    if (read(fd, memory, statbuf.st_size) == -1) {
        dprintf(2, "%s : %s\n", filepath, strerror(errno));
        return true;
    }

    close(fd);

    if (prog_size)
        *prog_size = statbuf.st_size;

    return false;
}

uint8_t generate_random_byte()
{
    return (uint8_t)(random() % 256);
}