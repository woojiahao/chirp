#include "chirp.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

ChirpConfig* parse_args(int argc, char* argv[]);
void usage(const char* prog);

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    // at minimum require the ROM
    usage(argv[0]);
    return 1;
  }

  ChirpConfig* config = parse_args(argc, argv);

  if (config->is_debug)
  {
    printf("welcome to chirp!\n");
    printf("starting chirp...\n");
    printf("loading ROM at path %s\n", config->rom_path);
  }

  // TODO: Add stats for how long the emulator is running for
  Chirp* chirp = chirp_new(config);

  if (config->is_debug)
  {
    printf("ROM loaded...\n");
    printf("creating window for chirp...\n");
    chirp_mem_view(chirp->mem);
  }

  SDLWindow* window = sdl_window_new();

  chirp_start_emulator_loop(chirp, window);

  if (config->is_debug)
  {
    printf("stopping chirp...\n");
  }

  // make sure to release all resources
  sdl_window_free(window);
  chirp_free(chirp);

  return 0;
}

void usage(const char* prog)
{
  fprintf(stderr,
          "usage: %s ROM [options]\n"
          "  [--debug]\n"
          "  [--shift-vx]\n"
          "  [--jump-with-nn]\n"
          "  [--set-registers-increment-index]\n"
          "  [--load-registers-increment-index]\n"
          "  [--has-audio]\n"
          "  [--cpu=N]\n",
          prog);
}

ChirpConfig* parse_args(int argc, char* argv[])
{
  ChirpConfig* config = malloc(sizeof(ChirpConfig));
  config->cpu_speed = 500;
  config->rom_path = "";

  config->is_debug = false;
  config->has_audio = false;
  config->jump_with_nn = false;
  config->load_registers_increment_index = false;
  config->set_registers_increment_index = false;
  config->shift_vx = false;

  // all argument parsing handled after
  static struct option long_opts[] = {
    {"debug", no_argument, 0, 0},
    {"shift-vx", no_argument, 0, 0},
    {"jump-with-nn", no_argument, 0, 0},
    {"set-registers-increment-index", no_argument, 0, 0},
    {"load-registers-increment-index", no_argument, 0, 0},
    {"audio", no_argument, 0, 0},
    {"cpu", optional_argument, 0, 0},
    {0, 0, 0, 0}, // sentinel to inform that the array has ended
  };

  int opt_index = 0;
  opterr = 1;
  int c;
  while ((c = getopt_long(argc, argv, "", long_opts, &opt_index)) != -1)
  {
    if (c == '?')
    {
      usage(argv[0]);
      exit(1);
    }

    if (c != 0)
    {
      // if not long option, ignore it entirely
      continue;
    }

    const char* name = long_opts[opt_index].name;
    const char* argval = optarg;

    if (strcmp(name, "debug") == 0) config->is_debug = true;
    else if (strcmp(name, "shift-vx") == 0) config->shift_vx = true;
    else if (strcmp(name, "jump-with-nn") == 0) config->jump_with_nn = true;
    else if (strcmp(name, "set-registers-increment-index") == 0) config->set_registers_increment_index = true;
    else if (strcmp(name, "load-registers-increment-index") == 0) config->load_registers_increment_index = true;
    else if (strcmp(name, "audio") == 0) config->has_audio = true;
    else if (strcmp(name, "cpu") == 0) config->cpu_speed = atoi(argval);
  }

  if (optind < argc)
  {
    config->rom_path = argv[optind++];
  }
  else
  {
    fprintf(stderr, "missing ROM path\n");
    exit(1);
  }

  if (optind < argc)
  {
    fprintf(stderr, "too many arguments\n");
    exit(1);
  }

  return config;
}
