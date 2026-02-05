#ifndef CHIRP_DISPLAY_H
#define CHIRP_DISPLAY_H

#include <stdbool.h>

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32
#define UPSCALE_FACTOR 16

#define WINDOW_WIDTH (DISPLAY_WIDTH * UPSCALE_FACTOR)
#define WINDOW_HEIGHT (DISPLAY_HEIGHT * UPSCALE_FACTOR)

typedef struct ChirpDisplay
{
  bool display[DISPLAY_HEIGHT][DISPLAY_WIDTH];
} ChirpDisplay;

bool chirp_get_pixel(ChirpDisplay *display, int x, int y);
void chirp_set_pixel(ChirpDisplay *display, int x, int y, bool state);
void chirp_flip_pixel(ChirpDisplay *display, int x, int y);

#endif // CHIRP_DISPLAY_H