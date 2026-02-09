#include "window.h"

#include <math.h>
#include <stdlib.h>

struct SDLBeeper
{
  SDL_AudioDeviceID dev;
  SDL_AudioStream* stream;

  int sample_rate;
  float freq;
  float volume;

  float* tone; // array of tones
  int tone_samples;
};

SDLBeeper* sdl_beeper_new()
{
  SDLBeeper* beeper = malloc(sizeof(SDLBeeper));

  SDL_AudioSpec spec;
  SDL_zero(spec);

  spec.freq = 48000;
  spec.format = SDL_AUDIO_F32;
  spec.channels = 1;
  beeper->dev = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec);
  if (beeper->dev == 0)
  {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "failed to open the audio device\n");
    exit(1);
  }

  beeper->stream = SDL_CreateAudioStream(&spec, &spec);
  if (beeper->stream == 0)
  {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "failed to create stream\n");
    exit(1);
  }
  SDL_BindAudioStream(beeper->dev, beeper->stream);
  SDL_ResumeAudioDevice(beeper->dev);

  beeper->sample_rate = spec.freq;
  beeper->freq = 440.0f;
  beeper->volume = 0.2f;

  const int ms = 50;
  beeper->tone_samples = (beeper->sample_rate * ms) / 1000;
  beeper->tone = (float*)SDL_malloc(beeper->tone_samples * sizeof(float));
  if (beeper->tone == NULL)
  {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "failed to allocate memory for beeper tone\n");
    exit(1);
  }

  for (int i = 0; i < beeper->tone_samples; i++)
  {
    float t = (float)i / (float)beeper->sample_rate;
    beeper->tone[i] = beeper->volume * sinf(2.0f * (float)M_PI * beeper->freq * t);
  }

  return beeper;
}

void sdl_beeper_free(SDLBeeper* beeper)
{
  SDL_free(beeper->tone);
  SDL_DestroyAudioStream(beeper->stream);
  SDL_CloseAudioDevice(beeper->dev);
  SDL_zero(*beeper);
}

void sdl_beeper_beep(SDLBeeper* beeper)
{
  const int want_bytes = (beeper->sample_rate / 10) * (int)sizeof(float);
  while (SDL_GetAudioStreamQueued(beeper->stream) < want_bytes)
  {
    SDL_PutAudioStreamData(beeper->stream, beeper->tone, beeper->tone_samples * (int)sizeof(float));
  }
}

void sdl_beeper_stop(SDLBeeper* beeper)
{
  SDL_ClearAudioStream(beeper->stream);
}

SDLWindow* sdl_window_new()
{
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == false)
  {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL could not initialize. SDL error: %s\n", SDL_GetError());
    exit(1);
  }

  SDL_Window* window = SDL_CreateWindow(
    "chirp", WINDOW_WIDTH, WINDOW_HEIGHT,
    SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_OPENGL);

  if (window == NULL)
  {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "could not create window. SDL error: %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    exit(1);
  }

  SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
  if (renderer == NULL)
  {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "could not create renderer. SDL error: %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    exit(1);
  }
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
  SDL_SetRenderVSync(renderer, 1);

  SDLBeeper* beeper = sdl_beeper_new();

  SDLWindow* chirp_window = malloc(sizeof(SDLWindow));
  if (chirp_window == NULL)
  {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "failed to allocate memory for ChirpWindow\n");
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    exit(1);
  }

  chirp_window->window = window;
  chirp_window->renderer = renderer;
  chirp_window->beeper = beeper;

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);

  return chirp_window;
}

void sdl_window_free(SDLWindow* window)
{
  SDL_DestroyWindow(window->window);
  SDL_DestroyRenderer(window->renderer);
  sdl_beeper_free(window->beeper);
  free(window);
  SDL_Quit();
}

void sdl_window_draw_display(SDLWindow* window, const ChirpDisplay* display)
{
  SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 255);
  SDL_RenderClear(window->renderer);

  SDL_SetRenderDrawColor(window->renderer, 255, 255, 255, 255);
  for (int y = 0; y < DISPLAY_HEIGHT; y++)
  {
    for (int x = 0; x < DISPLAY_WIDTH; x++)
    {
      if (chirp_display_get_pixel(display, x, y))
      {
        SDL_FRect rect = (SDL_FRect){
          .x = (float)x * UPSCALE_FACTOR,
          .y = (float)y * UPSCALE_FACTOR,
          .w = UPSCALE_FACTOR,
          .h = UPSCALE_FACTOR,
        };
        SDL_RenderFillRect(window->renderer, &rect);
      }
    }
  }
  SDL_RenderPresent(window->renderer);
}

void sdl_window_start_beep(SDLWindow* window)
{
  sdl_beeper_beep(window->beeper);
}

void sdl_window_stop_beep(SDLWindow* window)
{
  sdl_beeper_stop(window->beeper);
}
