#include "chirp.h"

int main()
{
  printf("welcome to chirp!\n");
  printf("starting chirp...\n");

  // TODO: Add stats for how long the emulator is running for
  Chirp *chirp = chirp_new();
  chirp_start_emulator_loop(chirp);

  return 0;
}