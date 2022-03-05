#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>

#include "disas.h"
#include "utils.h"
#include "chip8_engine.h"

#define COMMANDS_SIZE (2)

#define MAX_SCREEN_FPS              180
#define MAX_SCREEN_TICKS_PER_FRAME  (1000 / MAX_SCREEN_FPS)

typedef enum command {
    DISAS,
    INTERPRET,
    UNKNOWN
} command_t;

typedef uint32_t sdl_clock_t;

static const char *commands[COMMANDS_SIZE + 1] = {
        "disas",
        "interpret",
        NULL
};

static void reset_sdl_clock(sdl_clock_t *clock)
{
    *clock = SDL_GetTicks();
}

static sdl_clock_t get_elapsed(const sdl_clock_t *clock)
{
    return SDL_GetTicks() - (*clock);
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
    uint16_t pc = INITIAL_PROGRAM_COUNTER;

    if (!(buf = read_file_offset(*av, INITIAL_PROGRAM_COUNTER, &progsize, MAX_PROG_SIZE)))
        return 1;


    for (; (size_t)pc < progsize; pc += 2)
        disas(buf, pc , debug);

    free(buf);

    return 0;
}

/*
 *   1 2 3 4
 *   A Z E R
 *   Q S D F
 *   W X C V
 */
static const SDL_Keycode chip8_key_to_sdl_key[KEY_SIZE] = {
        SDLK_x, // 0x0
        SDLK_1, // 0x1
        SDLK_2, // 0x2
        SDLK_3, // 0x3
        SDLK_a, // 0x4
        SDLK_z, // 0x5
        SDLK_e, // 0x6
        SDLK_q, // 0x7
        SDLK_s, // 0x8
        SDLK_d, // 0x9
        SDLK_w, // 0xa
        SDLK_c, // 0xb
        SDLK_4, // 0xc
        SDLK_r, // 0xd
        SDLK_f, // 0xe
        SDLK_v  // 0xf
};

static uint8_t handle_keyboard_input(const SDL_Event *ev)
{
    for (uint8_t i = 0; i < KEY_SIZE; i++)
        if (ev->key.keysym.sym == chip8_key_to_sdl_key[i])
            return i;

    return KEY_SIZE;
}

static int interpret(int ac, const char **av)
{
    (void)ac;

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
    SDL_Texture *texture;

    if (SDL_Init(SDL_INIT_VIDEO))
        return 1;

    window = SDL_CreateWindow("Chip8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

    if (!window)
        return 1;

    if (!(renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED)))
        return 1;

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB332, SDL_TEXTUREACCESS_STATIC, WINDOW_WIDTH, WINDOW_HEIGHT);
    if (!texture)
        return 1;

    sdl_clock_t framerate_clock;
    sdl_clock_t cap_clock;

    int frame_counter = 0;

    reset_sdl_clock(&framerate_clock);

    bool running = true;

    while (running) {
        reset_sdl_clock(&cap_clock);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
                break;
            }

            bool is_key_pressed = event.type == SDL_KEYDOWN;
            if (is_key_pressed || event.type == SDL_KEYUP) {
                uint8_t key = handle_keyboard_input(&event);
                if (key < KEY_SIZE)
                    engine.keyboard[key] = is_key_pressed;
            }
        }

        update_chip8_engine(&engine);

        if (SDL_UpdateTexture(texture, NULL, engine.screen, WINDOW_WIDTH * sizeof(uint8_t)))
            return 1;

        if (SDL_RenderClear(renderer))
            return 1;
        if (SDL_RenderCopy(renderer, texture, NULL, NULL))
            return 1;
        SDL_RenderPresent(renderer);

        float avg_fps = ((float)frame_counter) / (((float)get_elapsed(&framerate_clock)) / 1000.f);

        if (avg_fps > 2000000)
            avg_fps = 0;

        printf("average fps = %f\n", avg_fps);

        frame_counter++;

        uint32_t frame_ticks = get_elapsed(&cap_clock);
        if (frame_ticks < MAX_SCREEN_TICKS_PER_FRAME)
            SDL_Delay(MAX_SCREEN_TICKS_PER_FRAME - frame_ticks);

    }

    SDL_DestroyTexture(texture);
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