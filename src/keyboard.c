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

uint8_t chirp_keyboard_read(const ChirpKeyboard* keyboard, const int addr)
{
  if (addr < 0 || addr >= CHIRP_KEYBOARD_SIZE) return 0;
  return keyboard->keyboard[addr & 0x0F];
}

void chirp_keyboard_write(ChirpKeyboard* keyboard, const int addr, const bool value)
{
  if (addr < 0 || addr >= CHIRP_KEYBOARD_SIZE) return;
  keyboard->keyboard[addr & 0x0F] = value;
}
