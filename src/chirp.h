#ifndef CHIRP_CHIRP_H
#define CHIRP_CHIRP_H

#include "memory.h"
#include "stack.h"
#include "registers.h"

#define CHIRP_INSTRUCTIONS_ADDR_START 0x200
#define CHIRP_INSTRUCTIONS_ADDR_END 0xFFF
#define CHIRP_INSTRUCTIONS_REGION_SIZE CHIRP_INSTRUCTIONS_ADDR_END - CHIRP_INSTRUCTIONS_ADDR_START

#define CHIRP_FONTS_COUNT 16
#define CHIRP_FONTS_BYTES CHIRP_FONTS_COUNT * 5
#define CHIRP_FONTS_ADDR_START 0x050
#define CHIRP_FONTS_ADDR_END 0x0A0
#define CHIRP_FONTS_REGION_SIZE CHIRP_FONTS_ADDR_END - CHIRP_FONTS_ADDR_START

// list taken from https://tobiasvl.github.io/blog/write-a-chip-8-emulator/#font
const static uint8_t CHIRP_FONTS[CHIRP_FONTS_BYTES] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

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

void chirp_load_rom(Chirp *chirp, const char *rom_path);
void chirp_load_fonts(Chirp *chirp);
void chirp_start_emulator_loop(Chirp *chirp);

#endif // CHIRP_CHIRP_H