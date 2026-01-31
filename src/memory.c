#include "memory.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char *join_string(const char *strings[], const char *separator, const size_t count)
{
  if (count == 0)
  {
    return strdup("");
  }

  size_t separator_length = strlen(separator);
  size_t total_length = 0;

  for (size_t i = 0; i < count; i++)
  {
    total_length += strlen(strings[i]);
  }

  total_length++;                                 // account for \0 character
  total_length += separator_length * (count - 1); // account for the separators in between

  char *joined_string = (char *)malloc(total_length);
  if (joined_string == NULL)
  {
    return NULL;
  }

  char *p = joined_string; // starts by pointing to the front of the memory space
  for (size_t i = 0; i < count; i++)
  {
    const char *current_string = strings[i];
    size_t current_string_length = strlen(current_string);
    memcpy(p, current_string, current_string_length);
    p += current_string_length;

    if (i + 1 < count)
    {
      memcpy(p, separator, separator_length);
      p += separator_length;
    }
  }

  *p = '\0'; // by this point, it'll point to the end of the string in the memory space

  return joined_string;
}

ChirpMemory *chirp_mem_new()
{
  // we don't allocate the inner array because it's fixed size
  ChirpMemory *mem = (ChirpMemory *)malloc(sizeof(ChirpMemory));

  // force initialize to 0
  for (int i = 0; i < CHIRP_MEMORY_SIZE; i++)
  {
    mem->mem[i] = 0;
  }

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

void chirp_mem_view(ChirpMemory *mem)
{
  // TODO: generify this function
  const uint8_t row_size = 50;

  // emulator section
  printf("=== emulator ===\n");
  for (uint16_t i = CHIRP_EMULATOR_ADDR_START; i <= CHIRP_EMULATOR_ADDR_END; i += row_size)
  {
    uint16_t remaining = CHIRP_EMULATOR_ADDR_END - i + 1;
    uint16_t current_row_size = remaining < row_size ? remaining : row_size;

    char block[current_row_size][3]; // 2 hex digits + null
    const char *block_ptrs[current_row_size];

    for (uint16_t j = 0; j < current_row_size; j++)
    {
      uint16_t idx = i + j;
      sprintf(block[j], "%02X", mem->mem[idx]);
      block_ptrs[j] = block[j];
    }

    char *row = join_string(block_ptrs, "  ", current_row_size);
    if (row)
    {
      printf("%04X: %s\n", i, row); // example output
      free(row);
    }
  }
  printf("=== emulator ===\n\n");

  // instructions section
  printf("=== instructions ===\n");
  for (uint16_t i = CHIRP_INSTRUCTIONS_ADDR_START; i <= CHIRP_INSTRUCTIONS_ADDR_END; i += row_size)
  {
    uint16_t remaining = CHIRP_INSTRUCTIONS_ADDR_END - i + 1;
    uint16_t current_row_size = remaining < row_size ? remaining : row_size;

    char block[current_row_size][3]; // 2 hex digits + null
    const char *block_ptrs[current_row_size];

    for (uint16_t j = 0; j < current_row_size; j++)
    {
      uint16_t idx = i + j;
      sprintf(block[j], "%02X", mem->mem[idx]);
      block_ptrs[j] = block[j];
    }

    char *row = join_string(block_ptrs, "  ", current_row_size);
    if (row)
    {
      printf("%04X: %s\n", i, row); // example output
      free(row);
    }
  }
  printf("=== instructions ===\n");
}
