#include "stack.h"
#include <stdio.h>
#include <stdlib.h>

struct ChirpStack
{
  uint16_t stack[CHIRP_STACK_SIZE];

  int current_size; // increments when the size of the stack increases
                    // (different from ptr to avoid confusion)
  int ptr;          // points to the next available position on the stack
};

static struct ChirpStack stack = {.current_size = 0, .ptr = 0};

void chirp_stack_push(uint16_t element)
{
  if (stack.current_size == CHIRP_STACK_SIZE)
  {
    // once at size, we should abort and error that there's a stack overflow
    printf("stack overflow, exceeds capacity %d\n", CHIRP_STACK_SIZE);
    exit(1);
  }

  stack.stack[stack.ptr++] = element;
  stack.current_size++;
}

uint16_t chirp_stack_pop()
{
  if (stack.current_size == 0)
  {
    // if there's nothing to pop, abort and error
    printf("stack is empty\n");
    exit(1);
  }

  uint16_t value = stack.stack[stack.ptr];

  stack.stack[stack.ptr--] = 0;
  stack.current_size--;

  return value;
}

uint16_t chirp_stack_peek()
{
  if (stack.current_size == 0)
  {
    // if there's nothing to pop, abort and error
    printf("stack is empty\n");
    exit(1);
  }

  return stack.stack[stack.ptr];
}

bool chirp_stack_is_empty()
{
  return stack.current_size == 0;
}

bool chirp_stack_is_full()
{
  return stack.current_size == CHIRP_STACK_SIZE;
}