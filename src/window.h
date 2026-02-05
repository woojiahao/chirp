#ifndef CHIRP_WINDOW_H
#define CHIRP_WINDOW_H

#include "SDL3/SDL.h"
#include "display.h"

typedef struct ChirpWindow
{
  SDL_Window *window;
  SDL_Renderer *renderer;
} ChirpWindow;

ChirpWindow *create_window();
void close_window(ChirpWindow *window);
void draw_display(ChirpWindow *window, ChirpDisplay *display);

#endif // CHIRP_WINDOW_H