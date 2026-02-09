#include "keyboard.h"
#include <stdlib.h>

ChirpKeyboard* chirp_keyboard_new()
{
  // we don't allocate the inner array because it's fixed size
  ChirpKeyboard* keyboard = malloc(sizeof(ChirpKeyboard));

  // force initialize to 0
  for (int i = 0; i < CHIRP_KEYBOARD_SIZE; i++)
  {
    keyboard->keyboard[i] = false;
  }

  return keyboard;
}

// TODO: Add guardrails to prevent out of bounds access
uint8_t chirp_keyboard_read(const ChirpKeyboard* keyboard, const int addr)
{
  return keyboard->keyboard[addr & 0x0FFF];
}

void chirp_keyboard_write(ChirpKeyboard* keyboard, const int addr, const bool value)
{
  keyboard->keyboard[addr & 0x0FFF] = value;
}
