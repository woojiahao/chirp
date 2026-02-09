#ifndef CHIRP_STACK_H
#define CHIRP_STACK_H

#include <stdint.h>
#include <stdbool.h>

#define CHIRP_STACK_SIZE 64

typedef struct ChirpStack
{
  uint16_t stack[CHIRP_STACK_SIZE];

  uint8_t current_size; // increments when the size of the stack increases (different from ptr to avoid confusion)
  uint8_t ptr;          // points to the next available position on the stack
} ChirpStack;

ChirpStack* chirp_stack_new();
void chirp_stack_push(ChirpStack* stack, uint16_t addr);
uint16_t chirp_stack_pop(ChirpStack* stack);
uint16_t chirp_stack_peek(const ChirpStack* stack);
bool chirp_stack_is_empty(const ChirpStack* stack);
bool chirp_stack_is_full(const ChirpStack* stack);

#endif // CHIRP_STACK_H
