#ifndef CHIRP_WINDOW_H
#define CHIRP_WINDOW_H

#include "SDL3/SDL.h"
#include "display.h"

typedef struct SDLBeeper SDLBeeper;

typedef struct SDLWindow
{
  SDL_Window* window;
  SDL_Renderer* renderer;
  SDLBeeper* beeper;
} SDLWindow;

SDLWindow* sdl_window_new();
void sdl_window_free(SDLWindow* window);
void sdl_window_draw_display(SDLWindow* window, const ChirpDisplay* display);
void sdl_window_start_beep(SDLWindow* window);
void sdl_window_stop_beep(SDLWindow* window);

#endif // CHIRP_WINDOW_H
