#include "registers.h"
#include <stdlib.h>

ChirpRegisters *chirp_mem_new()
{
  // we don't allocate the inner array because it's fixed size
  ChirpRegisters *mem = malloc(sizeof(ChirpRegisters));
  return mem;
}

uint8_t chirp_mem_read(ChirpRegisters *mem, uint16_t addr)
{
  return mem->mem[addr & 0x0FFF];
}

void chirp_mem_write(ChirpRegisters *mem, uint16_t addr, uint8_t value)
{
  mem->mem[addr & 0x0FFF] = value;
}