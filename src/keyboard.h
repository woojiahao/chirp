#ifndef CHIRP_KEYBOARD_H
#define CHIRP_KEYBOARD_H

#include <stdint.h>
#include <stdbool.h>

#define CHIRP_KEYBOARD_SIZE 16

typedef struct ChirpKeyboard
{
    bool keyboard[CHIRP_KEYBOARD_SIZE];
} ChirpKeyboard;

ChirpKeyboard* chirp_keyboard_new();
uint8_t chirp_keyboard_read(const ChirpKeyboard* keyboard, int addr);
void chirp_keyboard_write(ChirpKeyboard* keyboard, int addr, bool value);

#endif // CHIRP_KEYBOARD_H
