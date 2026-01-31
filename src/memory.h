#ifndef CHIRP_MEMORY_H
#define CHIRP_MEMORY_H

#include <stdint.h>

#define CHIRP_MEMORY_SIZE 4096

typedef struct ChirpMemory
{
    uint8_t mem[CHIRP_MEMORY_SIZE];
} ChirpMemory;

ChirpMemory *chirp_mem_new();
uint8_t chirp_mem_read(ChirpMemory *mem, uint16_t addr);
void chirp_mem_write(ChirpMemory *mem, uint16_t addr, uint8_t value);

#endif // CHIRP_MEMORY_H