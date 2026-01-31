#ifndef CHIRP_CHIRP_H
#define CHIRP_CHIRP_H

#include "memory.h"
#include "stack.h"
#include "registers.h"

#define CHIRP_INSTRUCTIONS_ADDR_START 0x200
#define CHIRP_INSTRUCTIONS_ADDR_END 0xFFF
#define CHIRP_INSTRUCTIONS_REGION_SIZE CHIRP_INSTRUCTIONS_ADDR_END - CHIRP_INSTRUCTIONS_ADDR_START
#define CHIRP_FONTS_ADDR_START 0x050
#define CHIRP_FONTS_ADDR_END 0x0A0
#define CHIRP_FONTS_REGION_SIZE CHIRP_FONTS_ADDR_END - CHIRP_FONTS_ADDR_START

typedef struct Chirp
{
  ChirpMemory *mem;
  ChirpStack *stack;
  ChirpRegisters *registers;

  uint8_t program_counter; // we can point to at most 4096 instructions (since the RAM is 4096)
  uint16_t index_register; // 16 bits to point to location
  uint8_t delay_timer;     // 8 bits to hold values from 0 to 60
  uint8_t sound_timer;     // 8 bits to hold values from 0 to 60
} Chirp;

Chirp *chirp_new(const char *rom_path);

void load_rom(Chirp *chirp, const char *rom_path);
void chirp_start_emulator_loop(Chirp *chirp);

#endif // CHIRP_CHIRP_H