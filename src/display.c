#include "display.h"

#define WINDOW_TITLE "Chip8"

#define MAX_SCREEN_FPS              200
#define MAX_SCREEN_TICKS_PER_FRAME  ((long int)(1000000L / MAX_SCREEN_FPS))

static bool sdl_error(const char *message)
{
    dprintf(2, "Error : %s : %s\n", message, SDL_GetError());
    return true;
}

bool init_display(display_t *d, bool log_framerate)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
        return sdl_error("unable to init SDL2");

    d->window = SDL_CreateWindow(
        WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    if (!d->window) return sdl_error("unable to create window");

    d->renderer = SDL_CreateRenderer(d->window, -1, SDL_RENDERER_ACCELERATED);
    if (!d->renderer) return sdl_error("unable to create renderer");

    d->texture = SDL_CreateTexture(
        d->renderer,
        SDL_PIXELFORMAT_RGB332,
        SDL_TEXTUREACCESS_STATIC,
        WINDOW_WIDTH,
        WINDOW_HEIGHT
    );
    if (!d->texture) return sdl_error("unable to create texture");


    d->frame_counter = 0;
    d->log_framerate = log_framerate;

    memset(&d->cap_clock, 0, sizeof(clock_t));
    reset_clock(&d->framerate_clock);

    return false;
}

/*
 *   1 2 3 4
 *   A Z E R
 *   Q S D F
 *   W X C V
 */
static const SDL_Keycode keys_map[KEY_SIZE] = {
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
        if (ev->key.keysym.sym == keys_map[i])
            return i;

    return KEY_SIZE;
}

bool poll_event(display_t *d, display_event_t *event)
{
    SDL_Event ev;

    reset_clock(&d->cap_clock);

    while (SDL_PollEvent(&ev)) {
        if (ev.type == SDL_QUIT)
            return false;

        event->key_pressed = ev.type == SDL_KEYDOWN;
        if (event->key_pressed || ev.type == SDL_KEYUP) {
            event->key = handle_keyboard_input(&ev);
            return true;
        }
    }

    event->key = KEY_SIZE;
    return true;
}

bool render(display_t *d, display_buffer_t *buf)
{
    float avg_fps = 0;
    uint32_t frame_ticks = 0;

    if (SDL_UpdateTexture(d->texture, NULL, buf, WINDOW_WIDTH * sizeof(uint8_t)))
        return sdl_error("unable to update texture");

    if (SDL_RenderClear(d->renderer))
        return sdl_error("unable to clear renderer");

    if (SDL_RenderCopy(d->renderer, d->texture, NULL, NULL))
        return sdl_error("unable to render");

    SDL_RenderPresent(d->renderer);

    avg_fps = (float)d->frame_counter / US_TO_S((float)get_elapsed(&d->framerate_clock));

    if (avg_fps > 2000000)
        avg_fps = 0;

    if (d->log_framerate)
        printf("average fps = %f\n", avg_fps);

    d->frame_counter++;

    frame_ticks = get_elapsed(&d->cap_clock);
    if (frame_ticks < MAX_SCREEN_TICKS_PER_FRAME)
        SDL_Delay(US_TO_MS(MAX_SCREEN_TICKS_PER_FRAME - frame_ticks));

    return false;
}

void destroy_display(display_t *d)
{
    SDL_DestroyTexture(d->texture);
    SDL_DestroyRenderer(d->renderer);
    SDL_DestroyWindow(d->window);
    SDL_Quit();
}