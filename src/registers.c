#include "registers.h"
#include <stdlib.h>

ChirpRegisters *chirp_registers_new()
{
  // we don't allocate the inner array because it's fixed size
  ChirpRegisters *registers = (ChirpRegisters *)malloc(sizeof(ChirpRegisters));

  // force initialize to 0
  for (int i = 0; i < CHIRP_REGISTERS_SIZE; i++)
  {
    registers->registers[i] = 0;
  }

  return registers;
}

uint8_t chirp_registers_read(ChirpRegisters *registers, uint16_t addr)
{
  return registers->registers[addr & 0x0FFF];
}

void chirp_registers_write(ChirpRegisters *registers, uint16_t addr, uint8_t value)
{
  registers->registers[addr & 0x0FFF] = value;
}