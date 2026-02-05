#include "chirp.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL3/SDL.h>

void chirp_load_rom(Chirp *chirp, const char *rom_path)
{
  FILE *rom = fopen(rom_path, "rb");
  if (rom != NULL)
  {
    fseek(rom, 0, SEEK_END);
    long rom_size = ftell(rom);
    rewind(rom);

    uint8_t *rom_contents = (uint8_t *)malloc(sizeof(uint8_t) * rom_size);
    if (rom_contents == NULL)
    {
      fclose(rom);
      printf("out of memory\n");
      exit(1);
    }

    fread(rom_contents, sizeof(uint8_t), rom_size, rom);
    if (CHIRP_INSTRUCTIONS_REGION_SIZE < rom_size)
    {
      fclose(rom);
      printf("ROM too large\n");
      exit(1);
    }

    for (int i = 0; i < rom_size; i++)
    {
      chirp_mem_write(chirp->mem, i + CHIRP_INSTRUCTIONS_ADDR_START, rom_contents[i]);
    }

    for (int i = 0; i < rom_size; i += 2)
    {
      printf("%04x\n", rom_contents[i] << 8 | rom_contents[i + 1]);
    }

    fclose(rom);
    free(rom_contents);
  }
  else
  {
    printf("ROM not found\n");
    exit(1);
  }
}

void chirp_load_fonts(Chirp *chirp)
{
  for (int i = 0; i < CHIRP_FONTS_BYTES; i++)
  {
    chirp_mem_write(chirp->mem, i + CHIRP_FONTS_ADDR_START, CHIRP_FONTS[i]);
  }
}

// loads all the necessary state for the CHIP-8 emulator
Chirp *chirp_new(const char *rom_path)
{
  Chirp *chirp = (Chirp *)malloc(sizeof(Chirp));

  // the memory all set to 0s
  chirp->mem = chirp_mem_new();
  chirp->stack = chirp_stack_new();
  chirp->registers = chirp_registers_new();
  chirp->display = chirp_display_new();
  chirp->keyboard = chirp_keyboard_new();

  chirp->is_running = true;
  chirp->need_draw_screen = true;
  chirp->is_waiting_for_key = false;

  chirp->delay_timer = 0;
  chirp->sound_timer = 0;
  chirp->index_register = 0;
  chirp->program_counter = (uint16_t)CHIRP_INSTRUCTIONS_ADDR_START;

  chirp->is_debug = true;

  chirp_load_rom(chirp, rom_path);
  chirp_load_fonts(chirp);

  return chirp;
}

void chirp_update_timers(Chirp *chirp)
{
  if (chirp->delay_timer > 0)
  {
    chirp->delay_timer--;
  }

  if (chirp->sound_timer > 0)
  {
    chirp->sound_timer--;
  }
}

uint16_t chirp_fetch(Chirp *chirp)
{
  uint8_t first_block = chirp_mem_read(chirp->mem, chirp->program_counter++);
  uint8_t second_block = chirp_mem_read(chirp->mem, chirp->program_counter++);

  // create the instruction using bit shifting
  uint16_t instruction = ((uint16_t)first_block << 8) | second_block;
  if (chirp->is_debug)
  {
    // printf("reading instruction %04x\n", instruction);
  }

  return instruction;
}

void chirp_execute(Chirp *chirp, uint16_t instruction)
{
  uint16_t opcode = instruction & 0xF000;
  uint8_t vx = (instruction & 0x0F00) >> 8;
  uint8_t vy = (instruction & 0x00F0) >> 4;
  uint8_t n = instruction & 0x000F;
  uint8_t nn = instruction & 0x00FF;
  uint16_t nnn = instruction & 0x0FFF;

  switch (opcode)
  {
  case 0x0000:
    switch (instruction & 0x0FFF)
    {
    case 0x00E0:
      // clear screen
      clear_display(chirp);
      return;
    case 0x00EE:
      // return from subroutine
      subroutine_return(chirp);
      return;
    default:
      printf("invalid instruction %04X\n", instruction);
      exit(1);
    }

  case 0x1000:
    jump(chirp, nnn);
    return;

  case 0x2000:
    subroutine_call(chirp, nnn);
    return;

  case 0x3000:
    skip_if_vx_eq_nn(chirp, vx, nn);
    return;

  case 0x4000:
    skip_if_vx_neq_nn(chirp, vx, nn);
    return;

  case 0x5000:
    skip_if_vx_eq_vy(chirp, vx, vy);
    return;

  case 0x6000:
    set_vx_eq_nn(chirp, vx, nn);
    return;

  case 0x7000:
    set_vx_eq_vx_plus_nn(chirp, vx, nn);
    return;

  case 0x8000:
    switch (n)
    {
    case 0x0:
      set_vx_to_vy(chirp, vx, vy);
      return;
    case 0x1:
      set_vx_eq_vx_or_vy(chirp, vx, vy);
      return;
    case 0x2:
      set_vx_eq_vx_and_vy(chirp, vx, vy);
      return;
    case 0x3:
      set_vx_eq_vx_xor_vy(chirp, vx, vy);
      return;
    case 0x4:
      set_vx_eq_vx_plus_vy(chirp, vx, vy);
      return;
    case 0x5:
      set_vx_eq_vx_minus_vy(chirp, vx, vy);
      return;
    case 0x6:
      set_vx_eq_vy_shift_right(chirp, vx, vy);
      return;
    case 0x7:
      set_vx_eq_vy_minus_vx(chirp, vx, vy);
      return;
    case 0xE:
      set_vx_eq_vy_shift_left(chirp, vx, vy);
      return;
    default:
      printf("invalid instruction %04X\n", instruction);
      exit(1);
    }
    return;

  case 0x9000:
    skip_if_vx_neq_vy(chirp, vx, vy);
    return;

  case 0xA000:
    set_index_eq_nnn(chirp, nnn);
    return;

  case 0xB000:
    jump_with_offset(chirp, nnn);
    return;

  case 0xC000:
    set_vx_eq_random(chirp, vx, nn);
    return;

  case 0xD000:
    draw(chirp, vx, vy, n);
    return;

  case 0xE000:
    switch (nn)
    {
    case 0x9E:
      skip_if_key_eq_vx(chirp, vx);
      return;
    case 0xA1:
      skip_if_key_neq_vx(chirp, vx);
      return;
    default:
      printf("invalid instruction %04X\n", instruction);
      exit(1);
    }
    return;

  case 0xF000:
    switch (nn)
    {
    case 0x07:
      set_vx_eq_delay(chirp, vx);
      return;
    case 0x15:
      set_delay_eq_vx(chirp, vx);
      return;
    case 0x18:
      set_sound_eq_vx(chirp, vx);
      return;
    case 0x1E:
      set_index_eq_index_plus_vx(chirp, vx);
      return;
    case 0x0A:
      get_key(chirp, vx);
      return;
    case 0x29:
      set_index_to_font(chirp, vx);
      return;
    case 0x33:
      binary_coded_decimal_conversion(chirp, vx);
      return;
    case 0x55:
      set_registers(chirp, vx);
      return;
    case 0x65:
      load_registers(chirp, vx);
      return;
    }
    return;
  }
}

void chirp_start_emulator_loop(Chirp *chirp, ChirpWindow *window)
{
  const double timer_tick_interval = 1.0 / 60.0;
  const double cpu_tick_interval = 1.0 / 500.0; // TODO: make this variable
  double timer_accumulator = 0.0;
  double cpu_accumulator = 0.0;

  clock_t last_time = clock();

  SDL_Event e;
  SDL_zero(e);

emulator_loop:
  while (chirp->is_running)
  {
    clock_t now = clock();
    double dt = (double)(now - last_time) / CLOCKS_PER_SEC;

    timer_accumulator += dt;
    cpu_accumulator += dt;

    while (SDL_PollEvent(&e))
    {
      switch (e.type)
      {
      case SDL_EVENT_QUIT:
        chirp->is_running = false;
        // update the timing to avoid exploding the timing
        last_time = now;
        goto emulator_loop;
        break;
      case SDL_EVENT_KEY_DOWN:
        switch (e.key.key)
        {
        case SDLK_ESCAPE:
          chirp->is_running = false;
          // update the timing to avoid exploding the timing
          last_time = now;
          goto emulator_loop;
          break;
        case SDLK_SPACE:
          chirp->is_paused = !chirp->is_paused;
          break;
        default:
          break;
        }

        for (int i = 0; i < CHIRP_KEYBOARD_SIZE; i++)
        {
          if (e.key.key == KEYMAP[i])
          {
            chirp_keyboard_write(chirp->keyboard, i, true);
            if (chirp->is_waiting_for_key)
            {
              chirp->is_waiting_for_key = false;
            }
          }
        }
        break;
      case SDL_EVENT_KEY_UP:
        for (int i = 0; i < CHIRP_KEYBOARD_SIZE; i++)
        {
          if (e.key.key == KEYMAP[i])
          {
            chirp_keyboard_write(chirp->keyboard, i, false);
          }
        }
        break;
      }
    }

    if (chirp->is_paused)
    {
      // update the timing to avoid exploding the timing
      last_time = now;
      continue;
    }

    // interleaving the updates to avoid cpu hogging
    while (cpu_accumulator >= cpu_tick_interval || timer_accumulator >= timer_tick_interval)
    {
      if (cpu_accumulator >= cpu_tick_interval)
      {
        if (!chirp->is_waiting_for_key)
        {
          // fetch instruction
          uint16_t instruction = chirp_fetch(chirp);
          // if (instruction == 0x0000)
          // {
          //   goto emulator_loop;
          // }

          // execute instruction
          chirp_execute(chirp, instruction);
        }
        cpu_accumulator -= cpu_tick_interval;
      }

      if (timer_accumulator >= timer_tick_interval)
      {
        // update timers
        chirp_update_timers(chirp);
        timer_accumulator -= timer_tick_interval;
      }
    }

    // render screen
    if (chirp->need_draw_screen)
    {
      draw_display(window, chirp->display);
      chirp->need_draw_screen = false;
    }

    // update the timing
    last_time = now;
  }
}
