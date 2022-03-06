#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

#include "disas.h"
#include "utils.h"
#include "display.h"

#define COMMANDS_SIZE 2

typedef enum command {
    DISAS,
    INTERPRET,
    UNKNOWN_COMMAND
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
    bool show_fps = false;
    bool disas = false;
    bool dump_regs = false;

    chip8_engine_t engine;
    display_t display;
    display_event_t ev = {KEY_SIZE, false};
    int exit_code = 0;

    for (int i = 1; i < ac; i++) {
        if (!strcmp(av[i], "--show-fps"))
            show_fps = true;
        if (!strcmp(av[i], "--disas"))
            disas = true;
        if (!strcmp(av[i], "--dump-regs"))
            dump_regs = true;
    }

    init_chip8_engine(&engine);
    if (load_file_to_memory(*av, engine.memory + INITIAL_PROGRAM_COUNTER, &engine.prog_size, MAX_PROG_SIZE))
        return 1;

    srandom(time(NULL));

    if (init_display(&display, show_fps))
        return 1;

    while (!exit_code) {
        if (!poll_event(&display, &ev))
            break;

        if (ev.key < KEY_SIZE) engine.keyboard[ev.key] = ev.key_pressed;

        update_chip8_engine(&engine, disas);

        if (dump_regs)
            chip8_dump_registers(&engine);

        if (engine.draw_flag) {
            exit_code = render(&display, &engine.screen);
            engine.draw_flag = false;
        }
    }

    destroy_display(&display);

    return exit_code;
}

#ifdef EMSCRIPTEN
typedef struct core
{
    chip8_engine_t *engine;
    display_t *display;
} core_t;

void main_loop(void *arg)
{
    core_t *core = (core_t *)arg;
    int exit_code;
    chip8_engine_t *engine = core->engine;
    display_t *display = core->display;
    display_event_t ev = {KEY_SIZE, false};

    if (!poll_event(display, &ev))
        return;

    if (ev.key < KEY_SIZE) engine->keyboard[ev.key] = ev.key_pressed;

    update_chip8_engine(engine, true);

    if (engine->draw_flag) {
        exit_code = render(display, &engine->screen);
        printf("exit_code = %d\n", exit_code);
        engine->draw_flag = false;
    }
}

static int wasm_run()
{
    display_t display;
    chip8_engine_t engine;
    core_t core = {
            &engine,
            &display
    };

    init_chip8_engine(core.engine);

    if (load_file_to_memory("./Pong.ch8", core.engine->memory + INITIAL_PROGRAM_COUNTER, &core.engine->prog_size, MAX_PROG_SIZE)) {
        return 1;
    }
    printf("OK\n");

    srandom(time(NULL));

    if (init_display(core.display, false))
        return 1;

    srandom(time(NULL));

    emscripten_set_main_loop_arg(&main_loop, &core, -1, 1);

    destroy_display(core.display);

    return 0;
}
#endif

int main(int ac, const char **av)
{
#ifdef EMSCRIPTEN
    return wasm_run();
#endif

    command_t cmd = UNKNOWN_COMMAND;

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