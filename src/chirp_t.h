#ifndef CHIRP_T_H
#define CHIRP_T_H

// holds definitions that need to be shared to avoid cyclic dependencies

#include "display.h"
#include "memory.h"
#include "stack.h"
#include "registers.h"
#include "keyboard.h"
#include "window.h"

// list taken from https://tobiasvl.github.io/blog/write-a-chip-8-emulator/#font
const static uint8_t CHIRP_FONTS[CHIRP_FONTS_BYTES] = {
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

// list taken from https://github.com/cookerlyk/Chip8/blob/master/src/chip8.h
const static uint8_t KEYMAP[CHIRP_KEYBOARD_SIZE] = {
  SDLK_X, // 0
  SDLK_1, // 1
  SDLK_2, // 2
  SDLK_3, // 3
  SDLK_Q, // 4
  SDLK_W, // 5
  SDLK_E, // 6
  SDLK_A, // 7
  SDLK_S, // 8
  SDLK_D, // 9
  SDLK_Z, // A
  SDLK_C, // B
  SDLK_4, // C
  SDLK_R, // D
  SDLK_F, // E
  SDLK_V  // F
};

typedef struct ChirpConfig
{
  const char* rom_path;                // path to the rom; must be set by user
  bool is_debug;                       // defaults to false
  bool shift_vx;                       // affects 8XY6 and 8XYE; defaults to false
  bool jump_with_vx;                   // affects BNNN; defaults to false
  bool set_registers_increment_index;  // affects FX55; defaults to false
  bool load_registers_increment_index; // affects FX65; defaults to false
  bool has_audio;                      // defaults to false
  int cpu_speed;                       // defaults to 500
} ChirpConfig;

typedef struct Chirp
{
  ChirpConfig* config;

  ChirpMemory* mem;
  ChirpStack* stack;
  ChirpRegisters* registers;
  ChirpDisplay* display;
  ChirpKeyboard* keyboard;

  uint16_t program_counter; // we can point to at most 4096 instructions (since the RAM is 4096)
  uint16_t index_register;  // 16 bits to point to location
  uint8_t delay_timer;      // 8 bits to hold values from 0 to 60
  uint8_t sound_timer;      // 8 bits to hold values from 0 to 60

  // flags to indicate state for running the machine
  bool is_running;
  bool is_paused;
  bool need_draw_screen;
} Chirp;

#endif
