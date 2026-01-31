#include "chirp.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void chirp_load_rom(Chirp *chirp, const char *rom_path)
{
  FILE *rom = fopen(rom_path, "rb");
  if (rom != NULL)
  {
    fseek(rom, 0, SEEK_END);
    long rom_size = ftell(rom);
    rewind(rom);

    uint8_t *rom_contents = (uint8_t *)malloc(sizeof(uint8_t) * rom_size);
    if (rom_contents == NULL)
    {
      fclose(rom);
      printf("out of memory\n");
      exit(1);
    }

    fread(rom_contents, sizeof(uint8_t), rom_size, rom);
    if (CHIRP_INSTRUCTIONS_REGION_SIZE < rom_size)
    {
      fclose(rom);
      printf("ROM too large\n");
      exit(1);
    }

    for (int i = 0; i < rom_size; i++)
    {
      chirp_mem_write(chirp->mem, i + CHIRP_INSTRUCTIONS_ADDR_START, rom_contents[i]);
    }

    fclose(rom);
    free(rom_contents);
  }
  else
  {
    printf("ROM not found\n");
    exit(1);
  }
}

void chirp_load_fonts(Chirp *chirp)
{
  for (int i = 0; i < CHIRP_FONTS_BYTES; i++)
  {
    chirp_mem_write(chirp->mem, i + CHIRP_FONTS_ADDR_START, CHIRP_FONTS[i]);
  }
}

// loads all the necessary state for the CHIP-8 emulator
Chirp *chirp_new(const char *rom_path)
{
  Chirp *chirp = (Chirp *)malloc(sizeof(Chirp));

  // the memory all set to 0s
  chirp->mem = chirp_mem_new();
  chirp->stack = chirp_stack_new();
  chirp->registers = chirp_registers_new();
  chirp->is_running = true;

  chirp->delay_timer = 0;
  chirp->sound_timer = 0;
  chirp->index_register = 0;
  chirp->program_counter = (uint8_t)CHIRP_INSTRUCTIONS_ADDR_START;

  chirp_load_rom(chirp, rom_path);
  chirp_load_fonts(chirp);

  return chirp;
}

void chirp_update_timers(Chirp *chirp)
{
  if (chirp->delay_timer > 0)
  {
    chirp->delay_timer--;
  }

  if (chirp->sound_timer > 0)
  {
    chirp->sound_timer--;
  }
}

void chirp_fetch(Chirp *chirp)
{
}

void chirp_execute(Chirp *chirp)
{
}

void chirp_start_emulator_loop(Chirp *chirp)
{
  const double tick_interval = 1.0 / 60.0; // 60 Hz where we tick 60 times a second
  double timer_accumulator = 0.0;

  clock_t last_time = clock();

  while (chirp->is_running)
  {
    clock_t now = clock();
    double dt = (double)(now - last_time) / CLOCKS_PER_SEC;

    timer_accumulator += dt;

    // fetch instruction
    chirp_fetch(chirp);

    // execute instruction
    chirp_execute(chirp);

    // update timers
    while (timer_accumulator >= tick_interval)
    {
      // keep updating the timer while there were more ticks than available
      chirp_update_timers(chirp);
      timer_accumulator -= tick_interval;
    }

    // render screen
  }
}
