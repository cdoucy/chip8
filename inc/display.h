#pragma once

#include <SDL2/SDL.h>
#include "chip8_engine.h"
#include "clock.h"

typedef struct display_s display_t;
typedef struct display_event_s display_event_t;

struct display_s {
    SDL_Window      *window;
    SDL_Renderer    *renderer;
    SDL_Texture     *texture;
    chip8_clock_t   framerate_clock;
    chip8_clock_t   cap_clock;
    int             frame_counter;
    bool            log_framerate;
};

struct display_event_s {
    uint8_t key;
    bool    key_pressed;
};

bool init_display(display_t *display, bool log_framerate);
bool poll_event(display_t *display, display_event_t *event);
bool render(display_t *display, display_buffer_t *buf);
void destroy_display(display_t *display);