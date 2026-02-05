#ifndef CHIRP_H
#define CHIRP_H

#include "chirp_t.h"
#include "instructions.h"

Chirp *chirp_new(const char *rom_path);

void chirp_load_rom(Chirp *chirp, const char *rom_path);
void chirp_load_fonts(Chirp *chirp);
void chirp_update_timers(Chirp *chirp);

uint16_t chirp_fetch(Chirp *chirp);                     // fetch instruction and increment PC
void chirp_execute(Chirp *chirp, uint16_t instruction); // execute instruction given

void chirp_start_emulator_loop(Chirp *chirp, ChirpWindow *window);

#endif // CHIRP_H