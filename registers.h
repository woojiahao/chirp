#ifndef CHIRP_REGISTERS_H
#define CHIRP_REGISTERS_H

#include <stdint.h>

#define CHIRP_REGISTERS_SIZE 16

typedef struct ChirpRegisters
{
    uint8_t mem[CHIRP_REGISTERS_SIZE];
} ChirpRegisters;

ChirpRegisters *chirp_registers_new();
uint8_t chirp_registers_read(ChirpRegisters *registers, uint16_t addr);
void chirp_registers_write(ChirpRegisters *registers, uint16_t addr, uint8_t value);

#endif // CHIRP_REGISTERS_H