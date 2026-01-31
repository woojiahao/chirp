//
// Created by Woo Jia Hao on 31/1/26.
//

#include "memory.h"
#include <stdlib.h>

struct ChirpMemory
{
  uint8_t mem[CHIRP_MEMORY_SIZE];
};

static struct ChirpMemory memory;

uint8_t chirp_mem_read(uint16_t addr)
{
  return memory.mem[addr & 0x0FFF];
}

void chirp_mem_write(uint16_t addr, uint8_t value)
{
  memory.mem[addr & 0x0FFF] = value;
}