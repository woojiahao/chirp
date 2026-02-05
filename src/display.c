#include "display.h"
#include <stdio.h>
#include <stdlib.h>

void _check_bounds(int x, int y)
{
  if (x < 0 || x >= DISPLAY_WIDTH || y < 0 || y >= DISPLAY_HEIGHT)
  {
    printf("invalid pixel position: x=%d, y=%d; display size is %d wide, %d tall", x, y, DISPLAY_WIDTH, DISPLAY_HEIGHT);
    exit(1);
  }
}

ChirpDisplay *chirp_display_new()
{
  ChirpDisplay *display = (ChirpDisplay *)malloc(sizeof(ChirpDisplay));

  for (int y = 0; y < DISPLAY_HEIGHT; y++)
  {
    for (int x = 0; x < DISPLAY_WIDTH; x++)
    {
      display->display[y][x] = false;
    }
  }

  return display;
}

bool chirp_display_get_pixel(ChirpDisplay *display, int x, int y)
{
  _check_bounds(x, y);
  return display->display[y][x];
}

void chirp_display_set_pixel(ChirpDisplay *display, int x, int y, bool state)
{
  _check_bounds(x, y);
  display->display[y][x] = state;
}

void chirp_display_flip_pixel(ChirpDisplay *display, int x, int y)
{
  _check_bounds(x, y);
  display->display[y][x] = !chirp_display_get_pixel(display, x, y);
}

void chirp_display_clear(ChirpDisplay *display)
{
  for (int y = 0; y < DISPLAY_HEIGHT; y++)
  {
    for (int x = 0; x < DISPLAY_WIDTH; x++)
    {
      chirp_display_set_pixel(display, x, y, false);
    }
  }
}