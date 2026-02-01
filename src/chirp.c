#include "chirp.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL3/SDL.h>

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

uint16_t chirp_fetch(Chirp *chirp)
{
  uint8_t first_block = chirp_mem_read(chirp->mem, chirp->program_counter++);
  uint8_t second_block = chirp_mem_read(chirp->mem, chirp->program_counter++);

  // create the instruction using bit shifting
  uint16_t instruction = ((uint16_t)first_block << 8) | second_block;

  return instruction;
}

void chirp_execute(Chirp *chirp, uint16_t instruction)
{
  uint16_t opcode = instruction & NIBBLE_ONE;

  switch (opcode)
  {
  case 0x0000:
    switch (instruction & 0x0FFF)
    {
    case 0x00E0:
      // clear screen
      // clear_screen(chirp);
      return;
    case 0x0EE:
      // return from subroutine
      // return_from_subroutine(chirp);
      return;
    default:
      printf("invalid instruction %04X\n", instruction);
      exit(1);
    }

  case 0x1000:
    return;

  case 0x2000:
    return;

  case 0x3000:
    return;

  case 0x4000:
    return;

  case 0x5000:
    return;

  case 0x6000:
    return;

  case 0x7000:
    return;

  case 0x8000:
    return;

  case 0x9000:
    return;

  case 0xA000:
    return;

  case 0xB000:
    return;

  case 0xC000:
    return;

  case 0xD000:
    return;

  case 0xE000:
    return;

  case 0xF000:
    return;
  }
}

void chirp_start_emulator_loop(Chirp *chirp)
{
  const double timer_tick_interval = 1.0 / 60.0;
  const double cpu_tick_interval = 1.0 / 500.0; // TODO: make this variable
  double timer_accumulator = 0.0;
  double cpu_accumulator = 0.0;

  clock_t last_time = clock();

  while (chirp->is_running)
  {
    clock_t now = clock();
    double dt = (double)(now - last_time) / CLOCKS_PER_SEC;

    timer_accumulator += dt;
    cpu_accumulator += dt;

    // interleaving the updates to avoid cpu hogging
    while (cpu_accumulator >= cpu_tick_interval || timer_accumulator >= timer_tick_interval)
    {
      if (cpu_accumulator >= cpu_tick_interval)
      {
        // fetch instruction
        uint16_t instruction = chirp_fetch(chirp);

        // execute instruction
        chirp_execute(chirp, instruction);
        cpu_accumulator -= cpu_tick_interval;
      }

      if (timer_accumulator >= timer_tick_interval)
      {
        // update timers
        chirp_update_timers(chirp);
        timer_accumulator -= timer_tick_interval;
      }
    }

    // render screen

    // update the timing
    last_time = now;
  }
}
