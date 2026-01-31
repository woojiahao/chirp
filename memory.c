#include "memory.h"
#include <stdlib.h>

ChirpMemory *chirp_mem_new()
{
  // we don't allocate the inner array because it's fixed size
  ChirpMemory *mem = malloc(sizeof(ChirpMemory));
  return mem;
}

uint8_t chirp_mem_read(ChirpMemory *mem, uint16_t addr)
{
  return mem->mem[addr & 0x0FFF];
}

void chirp_mem_write(ChirpMemory *mem, uint16_t addr, uint8_t value)
{
  mem->mem[addr & 0x0FFF] = value;
}