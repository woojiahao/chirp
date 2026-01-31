#include "chirp.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    printf("usage: %s <rom path>\n", argv[0]);
    return 1;
  }

  printf("welcome to chirp!\n");
  printf("starting chirp...\n");
  const char *path = argv[1];
  printf("loading ROM at path %s\n", path);

  // TODO: Add stats for how long the emulator is running for
  Chirp *chirp = chirp_new(path);
  chirp_mem_view(chirp->mem);
  chirp_start_emulator_loop(chirp);

  return 0;
}