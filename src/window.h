#ifndef CHIRP_WINDOW_H
#define CHIRP_WINDOW_H

#include "SDL3/SDL.h"
#include "display.h"
#include "keyboard.h"

// list taken from https://github.com/cookerlyk/Chip8/blob/master/src/chip8.h
const static uint8_t KEYMAP[CHIRP_KEYBOARD_SIZE] = {
    SDLK_X, // 0
    SDLK_1, // 1
    SDLK_2, // 2
    SDLK_3, // 3
    SDLK_Q, // 4
    SDLK_W, // 5
    SDLK_E, // 6
    SDLK_A, // 7
    SDLK_S, // 8
    SDLK_D, // 9
    SDLK_Z, // A
    SDLK_C, // B
    SDLK_4, // C
    SDLK_R, // D
    SDLK_F, // E
    SDLK_V  // F
};

typedef struct ChirpWindow
{
  SDL_Window *window;
  SDL_Renderer *renderer;
} ChirpWindow;

ChirpWindow *create_window();
void close_window(ChirpWindow *window);
void draw_display(ChirpWindow *window, ChirpDisplay *display);

#endif // CHIRP_WINDOW_H