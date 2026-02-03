#ifndef CHIRP_INSTRUCTIONS_H
#define CHIRP_INSTRUCTIONS_H

#include "chirp_t.h"

// all instruction names found here: https://tobiasvl.github.io/blog/write-a-chip-8-emulator/#instructions
// notes on instructions can be found here: https://woojiahao.notion.site/chirp-2f9f881eda0580158a73f6a072d7453b

// display
void clear_display(Chirp *chirp);                   // 00E0
void draw(Chirp *chirp, int vx, int vy, uint8_t n); // DXYN

// subroutines
void subroutine_return(Chirp *chirp);            // 00EE
void subroutine_call(Chirp *chirp, uint8_t nnn); // 2NNN

// jump
void jump(Chirp *chirp, uint8_t nnn);             // 1NNN
void jump_with_offset(Chirp *chirp, uint8_t nnn); // BNNN

// skip_if
void skip_if_vx_eq_nn(Chirp *chirp, int vx, uint8_t nn);  // 3XNN
void skip_if_vx_neq_nn(Chirp *chirp, int vx, uint8_t nn); // 4XNN
void skip_if_vx_eq_vy(Chirp *chirp, int vx, int vy);      // 5XY0
void skip_if_vx_neq_vy(Chirp *chirp, int vx, int vy);     // 9XY0
void skip_if_key_eq_vx(Chirp *chirp, int vx);             // EX9E
void skip_if_key_neq_vx(Chirp *chirp, int vx);            // EX9E

// set_vx
void set_vx_eq_nn(Chirp *chirp, int vx, uint8_t nn);         // 6XNN
void set_vx_eq_vx_plus_nn(Chirp *chirp, int vx, uint8_t nn); // 7XNN
void set_vx_to_vy(Chirp *chirp, int vx, int vy);             // 8XY0
void set_vx_eq_vx_or_vy(Chirp *chirp, int vx, int vy);       // 8XY1
void set_vx_eq_vx_and_vy(Chirp *chirp, int vx, int vy);      // 8XY2
void set_vx_eq_vx_xor_vy(Chirp *chirp, int vx, int vy);      // 8XY3
void set_vx_eq_vx_plus_vy(Chirp *chirp, int vx, int vy);     // 8XY4
void set_vx_eq_vx_minus_vy(Chirp *chirp, int vx, int vy);    // 8XY5
void set_vx_eq_vx_minus_vy(Chirp *chirp, int vx, int vy);    // 8XY5
void set_vx_eq_vy_shift_right(Chirp *chirp, int vx, int vy); // 8XY6
void set_vx_eq_vy_minus_vx(Chirp *chirp, int vx, int vy);    // 8XY7
void set_vx_eq_vy_shift_left(Chirp *chirp, int vx, int vy);  // 8XYE
void set_vx_eq_random(Chirp *chirp, int vx, uint8_t nn);     // CXNN
void set_vx_eq_delay(Chirp *chirp, int vx);                  // FX07

// set index
void set_index_eq_nnn(Chirp *chirp, uint8_t nnn); // ANNN

// registers
void set_registers(Chirp *chirp, int vx);  // FX55
void load_registers(Chirp *chirp, int vx); // FX65

// timers
void get_key(Chirp *chirp, int vx);         // FX0A
void set_delay_eq_vx(Chirp *chirp, int vx); // FX15
void set_sound_eq_vx(Chirp *chirp, int vx); // FX18

// set index
void set_I_eq_I_plus_vx(Chirp *chirp, int vx); // FX1E
void set_I_eq_vx(Chirp *chirp, int vx);        // FX29

// others
void binary_coded_decimal_conversion(Chirp *chirp, int vx); // FX33

#endif // CHIRP_INSTRUCTIONS_H