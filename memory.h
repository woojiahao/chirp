//
// Created by Woo Jia Hao on 31/1/26.
//

#ifndef CHIRP_MEMORY_H
#define CHIRP_MEMORY_H

#include <stdint.h>
#define CHIRP_MEMORY_SIZE 4096

typedef struct ChirpMemory ChirpMemory;

uint8_t chirp_mem_read(uint16_t addr);
void chirp_mem_write(uint16_t addr, uint8_t value);

#endif // CHIRP_MEMORY_H