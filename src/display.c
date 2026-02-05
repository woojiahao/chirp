#include "display.h"
#include <stdio.h>
#include <stdlib.h>

void _check_bounds(int x, int y)
{
  if (x < 0 || x >= DISPLAY_WIDTH || y < 0 || y >= DISPLAY_HEIGHT)
  {
    printf("invalid pixel position: x=%d, y=%d; display size is %s wide, %s tall", x, y, DISPLAY_WIDTH, DISPLAY_HEIGHT);
    exit(1);
  }
}

bool chirp_get_pixel(ChirpDisplay *display, int x, int y)
{
  _check_bounds(x, y);
  return display->display[y][x];
}

void chirp_set_pixel(ChirpDisplay *display, int x, int y, bool state)
{
  _check_bounds(x, y);
  display->display[y][x] = state;
}

void chirp_flip_pixel(ChirpDisplay *display, int x, int y)
{
  _check_bounds(x, y);
  display->display[y][x] = !chirp_get_pixel(display, x, y);
}
