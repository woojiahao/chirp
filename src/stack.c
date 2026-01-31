#include "stack.h"
#include <stdio.h>
#include <stdlib.h>

ChirpStack *chirp_stack_new()
{
  // we don't allocate the inner array because it's fixed size
  ChirpStack *chirp_stack = (ChirpStack *)malloc(sizeof(ChirpStack));
  chirp_stack->current_size = 0;
  chirp_stack->ptr = 0;

  // force initialize to 0
  for (int i = 0; i < CHIRP_STACK_SIZE; i++)
  {
    chirp_stack->stack[i] = 0;
  }

  return chirp_stack;
}

void chirp_stack_push(ChirpStack *stack, uint16_t element)
{
  if (stack->current_size == CHIRP_STACK_SIZE)
  {
    // once at size, we should abort and error that there's a stack overflow
    printf("stack overflow, exceeds capacity %d\n", CHIRP_STACK_SIZE);
    exit(1);
  }

  stack->stack[stack->ptr++] = element;
  stack->current_size++;
}

uint16_t chirp_stack_pop(ChirpStack *stack)
{
  if (stack->current_size == 0)
  {
    // if there's nothing to pop, abort and error
    printf("stack->is empty\n");
    exit(1);
  }

  uint16_t value = stack->stack[stack->ptr];

  stack->stack[stack->ptr--] = 0;
  stack->current_size--;

  return value;
}

uint16_t chirp_stack_peek(ChirpStack *stack)
{
  if (stack->current_size == 0)
  {
    // if there's nothing to pop, abort and error
    printf("stack->is empty\n");
    exit(1);
  }

  return stack->stack[stack->ptr];
}

bool chirp_stack_is_empty(ChirpStack *stack)
{
  return stack->current_size == 0;
}

bool chirp_stack_is_full(ChirpStack *stack)
{
  return stack->current_size == CHIRP_STACK_SIZE;
}