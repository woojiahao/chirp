#include "window.h"
#include <stdlib.h>

ChirpWindow *create_window()
{
  if (SDL_Init(SDL_INIT_VIDEO) == false)
  {
    SDL_Log("SDL could not initialize. SDL error: %s\n", SDL_GetError());
    exit(1);
  }

  SDL_Window *window = SDL_CreateWindow("chirp", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_OPENGL);
  if (window == NULL)
  {
    SDL_Log("could not create window. SDL error: %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    exit(1);
  }

  SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
  if (renderer == NULL)
  {
    SDL_Log("could not create renderer. SDL error: %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    exit(1);
  }

  ChirpWindow *chirp_window = (ChirpWindow *)malloc(sizeof(ChirpWindow));
  if (chirp_window == NULL)
  {
    SDL_Log("failed to allocate memory for ChirpWindow\n");
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    exit(1);
  }

  chirp_window->window = window;
  chirp_window->renderer = renderer;

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);

  return chirp_window;
}

void close_window(ChirpWindow *window)
{
  SDL_DestroyWindow(window->window);
  SDL_DestroyRenderer(window->renderer);
  free(window);
  SDL_Quit();
}

void draw_pixel(ChirpWindow *window, int x, int y, bool pixel)
{
  // in CHIP-8, the pixels don't wrap around, so we just ignore anything that exceeds the bounds
  if (pixel)
  {
    SDL_FRect rect = (SDL_FRect){.x = (x + y) * UPSCALE_FACTOR, .y = y * UPSCALE_FACTOR, .w = UPSCALE_FACTOR, .h = UPSCALE_FACTOR};
    SDL_SetRenderDrawColor(window->renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(window->renderer, &rect);
  }
  else
  {
    SDL_FRect rect = (SDL_FRect){.x = (x + y) * UPSCALE_FACTOR, .y = y * UPSCALE_FACTOR, .w = UPSCALE_FACTOR, .h = UPSCALE_FACTOR};
    SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(window->renderer, &rect);
  }
}

void draw_display(ChirpWindow *window, ChirpDisplay *display)
{
  for (int y = 0; y < DISPLAY_HEIGHT; y++)
  {
    for (int x = 0; x < DISPLAY_WIDTH; x++)
    {
      draw_pixel(window, x, y, chirp_display_get_pixel(display, x, y));
    }
  }
  SDL_RenderPresent(window->renderer);
}