#ifndef CHIRP_H
#define CHIRP_H

#include "chirp_t.h"

Chirp* chirp_new(ChirpConfig* config);
void chirp_free(Chirp* chirp);
void chirp_start_emulator_loop(Chirp* chirp, SDLWindow* window);

#endif // CHIRP_H
