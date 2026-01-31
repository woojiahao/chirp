#include "chirp.h"
#include <stdio.h>

Chirp *chirp_new()
{
    Chirp *chirp = malloc(sizeof(Chirp));
    chirp->mem = chirp_mem_new();
    chirp->stack = chirp_stack_new();
    chirp->registers = chirp_registers_new();
    chirp->delay_timer = 60;
    chirp->sound_timer = 60;
    chirp->index_register = 0;
    chirp->program_counter = CHIRP_PC_START;
    return chirp;
}

void chirp_start_emulator_loop(Chirp *chirp)
{
}
