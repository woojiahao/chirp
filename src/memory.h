#ifndef CHIRP_MEMORY_H
#define CHIRP_MEMORY_H

#include <stdint.h>

#define CHIRP_MEMORY_SIZE 4096

#define CHIRP_EMULATOR_ADDR_START 0x000
#define CHIRP_EMULATOR_ADDR_END 0x1FF

#define CHIRP_FONTS_COUNT 16
#define CHIRP_FONTS_BYTES CHIRP_FONTS_COUNT * 5 // every font is 5 bytes
#define CHIRP_FONTS_ADDR_START 0x050
#define CHIRP_FONTS_ADDR_END 0x0A0
#define CHIRP_FONTS_REGION_SIZE CHIRP_FONTS_ADDR_END - CHIRP_FONTS_ADDR_START

#define CHIRP_INSTRUCTIONS_ADDR_START 0x200
#define CHIRP_INSTRUCTIONS_ADDR_END 0xFFF
#define CHIRP_INSTRUCTIONS_REGION_SIZE CHIRP_INSTRUCTIONS_ADDR_END - CHIRP_INSTRUCTIONS_ADDR_START

typedef struct ChirpMemory
{
    uint8_t mem[CHIRP_MEMORY_SIZE];
} ChirpMemory;

ChirpMemory *chirp_mem_new();
uint8_t chirp_mem_read(ChirpMemory *mem, uint16_t addr);
void chirp_mem_write(ChirpMemory *mem, uint16_t addr, uint8_t value);
void chirp_mem_view(ChirpMemory *mem);

#endif // CHIRP_MEMORY_H