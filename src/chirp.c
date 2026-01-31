#include "chirp.h"
#include <stdio.h>
#include <stdlib.h>

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
  Chirp *chirp = malloc(sizeof(Chirp));
  chirp->mem = chirp_mem_new();
  chirp->stack = chirp_stack_new();
  chirp->registers = chirp_registers_new();
  chirp->delay_timer = 60;
  chirp->sound_timer = 60;
  chirp->index_register = 0;
  chirp->program_counter = (uint8_t)CHIRP_INSTRUCTIONS_ADDR_START;

  chirp_load_rom(chirp, rom_path);
  chirp_load_fonts(chirp);

  for (int i = CHIRP_FONTS_ADDR_START; i < CHIRP_FONTS_ADDR_END; i++)
  {
    printf("%d\n", chirp_mem_read(chirp->mem, i));
  }

  printf("next\n");

  for (int i = CHIRP_INSTRUCTIONS_ADDR_START; i < CHIRP_INSTRUCTIONS_ADDR_END; i++)
  {
    printf("%d\n", chirp_mem_read(chirp->mem, i));
  }

  return chirp;
}

void chirp_start_emulator_loop(Chirp *chirp)
{
  // while (true)
  // {
  //   // fetch instruction

  //   // decode instruction

  //   // execute instruction

  //   // update timers
  // }
}
