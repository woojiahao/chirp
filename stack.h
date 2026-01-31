//
// Created by Woo Jia Hao on 31/1/26.
//

#ifndef CHIRP_STACK_H
#define CHIRP_STACK_H

#include <stdint.h>
#include <stdbool.h>
#define CHIRP_STACK_SIZE 64

typedef struct ChirpStack ChirpStack;

void chirp_stack_push(uint16_t addr);
uint16_t chirp_stack_pop();
uint16_t chirp_stack_peek();
bool chirp_stack_is_empty();
bool chirp_stack_is_full();

#endif // CHIRP_STACK_H