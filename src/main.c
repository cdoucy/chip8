#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>

#include "disas.h"
#include "utils.h"
#include "chip8_engine.h"

#define WINDOW_SCALE 12

#define WINDOW_WIDTH    (CHIP8_WINDOW_WIDTH  *  WINDOW_SCALE)
#define WINDOW_HEIGHT   (CHIP8_WINDOW_HEIGHT *  WINDOW_SCALE)

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

static void draw_points(SDL_Renderer *renderer, int x, int y, int scale)
{
    int x_scale = x * scale;
    int y_scale = y * scale;

    for (int i = y_scale - scale; i < y_scale; i++)
        for (int j = x_scale - scale; j < x_scale; j++)
            SDL_RenderDrawPoint(renderer, j, i);
}

static void update_window(chip8_engine_t *engine, SDL_Renderer *renderer, int scale)
{
    update_chip8_engine(engine);

    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);

    for (uint8_t i = 0; i < CHIP8_WINDOW_HEIGHT; i++) {
        for (uint8_t j = 0; j < CHIP8_WINDOW_WIDTH; j++) {
            if (!(engine->screen[i] >> j & 1))
                continue;

            draw_points(renderer, j, i, scale);
        }
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xff);
    SDL_RenderPresent(renderer);
}

static int interpret(int ac, const char **av)
{
    chip8_engine_t engine;
    bool error = false;

    init_chip8_engine(&engine);
    error = load_file_to_memory(*av, engine.memory + INITIAL_PROGRAM_COUNTER, &engine.prog_size, MAX_PROG_SIZE);

    if (error)
        return 1;

    srandom(time(NULL));

    SDL_Event event;
    SDL_Renderer *renderer;
    SDL_Window *window;

    if (SDL_Init(SDL_INIT_VIDEO))
        return 1;

    if (SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer))
        return 1;

    bool running = true;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
                break;
            }
        }
        update_window(&engine, renderer, WINDOW_SCALE);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

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