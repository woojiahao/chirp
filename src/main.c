#include "chirp.h"
#include <stdio.h>
#include <stdlib.h>

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

  printf("ROM loaded...\n");
  printf("creating window for chirp...\n");

  ChirpWindow *window = create_window();

  chirp_mem_view(chirp->mem);
  chirp_start_emulator_loop(chirp, window);

  printf("stopping chirp...\n");

  // make sure to release all resources
  close_window(window);
  free(chirp->mem);
  free(chirp->registers);
  free(chirp->stack);
  free(chirp->display);
  free(chirp);

  return 0;
}