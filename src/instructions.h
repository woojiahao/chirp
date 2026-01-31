#ifndef CHIRP_INSTRUCTIONS_H
#define CHIRP_INSTRUCTIONS_H

#include "chirp_t.h"

// all instruction names found here: https://tobiasvl.github.io/blog/write-a-chip-8-emulator/#instructions
void clear_screen(Chirp *chirp);
void jump(Chirp *chirp);
void set_register_x(Chirp *chirp);
void add_value_to_register_x(Chirp *chirp);
void set_index_register(Chirp *chirp);
void display(Chirp *chirp);
void return_from_subroutine(Chirp *chirp);

#endif // CHIRP_INSTRUCTIONS_H