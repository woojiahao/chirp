#ifndef CHIRP_INSTRUCTIONS_H
#define CHIRP_INSTRUCTIONS_H

#include "chirp_t.h"

// all instruction names found here: https://tobiasvl.github.io/blog/write-a-chip-8-emulator/#instructions
// notes on instructions can be found here: https://woojiahao.notion.site/chirp-2f9f881eda0580158a73f6a072d7453b

// display
void clear_display(Chirp* chirp);                 // 00E0
void draw(Chirp* chirp, int x, int y, uint8_t n); // DXYN

// subroutines
void subroutine_return(Chirp* chirp);             // 00EE
void subroutine_call(Chirp* chirp, uint16_t nnn); // 2NNN

// jump
void jump(Chirp* chirp, uint16_t nnn);                           // 1NNN
void jump_with_offset_nnn(Chirp* chirp, uint16_t nnn);           // BNNN (variation 1)
void jump_with_offset_nnn_vx(Chirp* chirp, int x, uint16_t nnn); // BNNN (variation 2)

// skip_if
void skip_if_vx_eq_nn(Chirp* chirp, int x, uint8_t nn);  // 3XNN
void skip_if_vx_neq_nn(Chirp* chirp, int x, uint8_t nn); // 4XNN
void skip_if_vx_eq_vy(Chirp* chirp, int x, int y);       // 5XY0
void skip_if_vx_neq_vy(Chirp* chirp, int x, int y);      // 9XY0
void skip_if_key_vx_pressed(Chirp* chirp, int x);        // EX9E
void skip_if_key_vx_not_pressed(Chirp* chirp, int x);    // EXA1

// set_vx
void set_vx_eq_nn(Chirp* chirp, int x, uint8_t nn);         // 6XNN
void set_vx_eq_vx_plus_nn(Chirp* chirp, int x, uint8_t nn); // 7XNN
void set_vx_eq_vy(Chirp* chirp, int x, int y);              // 8XY0
void set_vx_eq_vx_or_vy(Chirp* chirp, int x, int y);        // 8XY1
void set_vx_eq_vx_and_vy(Chirp* chirp, int x, int y);       // 8XY2
void set_vx_eq_vx_xor_vy(Chirp* chirp, int x, int y);       // 8XY3
void set_vx_eq_vx_plus_vy(Chirp* chirp, int x, int y);      // 8XY4
void set_vx_eq_vx_minus_vy(Chirp* chirp, int x, int y);     // 8XY5
void set_vx_eq_vy_shift_right(Chirp* chirp, int x, int y);  // 8XY6 (variation 1)
void set_vx_eq_vx_shift_right(Chirp* chirp, int x);         // 8XY6 (variation 2)
void set_vx_eq_vy_minus_vx(Chirp* chirp, int x, int y);     // 8XY7
void set_vx_eq_vy_shift_left(Chirp* chirp, int x, int y);   // 8XYE (variation 1)
void set_vx_eq_vx_shift_left(Chirp* chirp, int x);          // 8XYE (variation 2)
void set_vx_eq_random(Chirp* chirp, int x, uint8_t nn);     // CXNN
void set_vx_eq_delay(Chirp* chirp, int x);                  // FX07

// set index
void set_index_eq_nnn(Chirp* chirp, uint16_t nnn);    // ANNN
void set_index_eq_index_plus_vx(Chirp* chirp, int x); // FX1E
void set_index_eq_font(Chirp* chirp, int x);          // FX29

// registers
void set_registers(Chirp* chirp, int x);      // FX55 (variation 1)
void set_registers_inc(Chirp* chirp, int x);  // FX55 (variation 2)
void load_registers(Chirp* chirp, int x);     // FX65 (variation 1)
void load_registers_inc(Chirp* chirp, int x); // FX65 (variation 2)

// timers
void set_delay_eq_vx(Chirp* chirp, int x); // FX15
void set_sound_eq_vx(Chirp* chirp, int x); // FX18

// others
void get_key(Chirp* chirp, int x);                         // FX0A
void binary_coded_decimal_conversion(Chirp* chirp, int x); // FX33

#endif // CHIRP_INSTRUCTIONS_H
