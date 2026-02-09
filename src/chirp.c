#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>

#include "chirp.h"
#include "instructions.h"

void chirp_load_rom(Chirp* chirp)
{
  FILE* rom = fopen(chirp->config->rom_path, "rb");
  if (rom != NULL)
  {
    fseek(rom, 0, SEEK_END);
    const long rom_size = ftell(rom);
    rewind(rom);

    uint8_t* rom_contents = malloc(sizeof(uint8_t) * rom_size);
    if (rom_contents == NULL)
    {
      fclose(rom);
      fprintf(stderr, "out of memory\n");
      exit(1);
    }

    fread(rom_contents, sizeof(uint8_t), rom_size, rom);
    if (CHIRP_INSTRUCTIONS_REGION_SIZE < rom_size)
    {
      fclose(rom);
      fprintf(stderr, "ROM too large\n");
      exit(1);
    }

    for (int i = 0; i < rom_size; i++)
    {
      chirp_mem_write(chirp->mem, i + CHIRP_INSTRUCTIONS_ADDR_START, rom_contents[i]);
    }

    fclose(rom);
    free(rom_contents);
  }
  else
  {
    fprintf(stderr, "ROM not found\n");
    exit(1);
  }
}

void chirp_load_fonts(Chirp* chirp)
{
  for (int i = 0; i < CHIRP_FONTS_BYTES; i++)
  {
    chirp_mem_write(chirp->mem, i + CHIRP_FONTS_ADDR_START, CHIRP_FONTS[i]);
  }
}

// loads all the necessary state for the CHIP-8 emulator
Chirp* chirp_new(ChirpConfig* config)
{
  Chirp* chirp = malloc(sizeof(Chirp));

  chirp->config = config;

  // the memory all set to 0s
  chirp->mem = chirp_mem_new();
  chirp->stack = chirp_stack_new();
  chirp->registers = chirp_registers_new();
  chirp->display = chirp_display_new();
  chirp->keyboard = chirp_keyboard_new();

  chirp->is_running = true;
  chirp->is_paused = false;
  chirp->need_draw_screen = true; // draw the very first screen
  chirp->is_waiting_for_key = false;

  chirp->delay_timer = 0;
  chirp->sound_timer = 0;
  chirp->index_register = 0;
  chirp->program_counter = (uint16_t)CHIRP_INSTRUCTIONS_ADDR_START;
  chirp->waiting_key_register = -1; // -1 to indicate that this is empty

  chirp_load_rom(chirp);
  chirp_load_fonts(chirp);

  return chirp;
}

void chirp_update_timers(Chirp* chirp, SDLWindow* window)
{
  if (chirp->delay_timer > 0)
  {
    chirp->delay_timer--;
  }

  if (chirp->sound_timer > 0)
  {
    if (chirp->config->has_audio)
    {
      sdl_window_start_beep(window);
    }
    chirp->sound_timer--;
  }
  else
  {
    if (chirp->config->has_audio)
    {
      sdl_window_stop_beep(window);
    }
  }
}

void chirp_free(Chirp* chirp)
{
  free(chirp->mem);
  free(chirp->registers);
  free(chirp->stack);
  free(chirp->display);
  free(chirp->config);
  free(chirp);
}

uint16_t chirp_fetch(Chirp* chirp)
{
  const uint8_t first_block = chirp_mem_read(chirp->mem, chirp->program_counter++);
  const uint8_t second_block = chirp_mem_read(chirp->mem, chirp->program_counter++);

  // create the instruction using bit shifting
  const uint16_t instruction = ((uint16_t)first_block << 8) | second_block;

  return instruction;
}

void chirp_execute(Chirp* chirp, const uint16_t instruction)
{
  const uint16_t opcode = instruction & 0xF000;
  const uint8_t x = (instruction & 0x0F00) >> 8;
  const uint8_t y = (instruction & 0x00F0) >> 4;
  const uint8_t n = instruction & 0x000F;
  const uint8_t nn = instruction & 0x00FF;
  const uint16_t nnn = instruction & 0x0FFF;

  if (chirp->config->is_debug)
  {
    if (chirp_stack_is_empty(chirp->stack))
    {
      SDL_Log(
        "executing instruction %04X with PC = %04X and EMPTY STACK\n",
        instruction,
        chirp->program_counter);
    }
    else
    {
      SDL_Log(
        "executing instruction %04X with PC = %04X and top of stack as %04X\n",
        instruction,
        chirp->program_counter,
        chirp_stack_peek(chirp->stack));
    }
  }

  switch (opcode)
  {
  case 0x0000:
    switch (nnn)
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
      fprintf(stderr, "invalid instruction %04X\n", instruction);
      exit(1);
    }

  case 0x1000:
    jump(chirp, nnn);
    return;

  case 0x2000:
    subroutine_call(chirp, nnn);
    return;

  case 0x3000:
    skip_if_vx_eq_nn(chirp, x, nn);
    return;

  case 0x4000:
    skip_if_vx_neq_nn(chirp, x, nn);
    return;

  case 0x5000:
    if (n != 0x0)
    {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR, "invalid instruction %04X\n", instruction);
      exit(1);
    }
    skip_if_vx_eq_vy(chirp, x, y);
    return;

  case 0x6000:
    set_vx_eq_nn(chirp, x, nn);
    return;

  case 0x7000:
    set_vx_eq_vx_plus_nn(chirp, x, nn);
    return;

  case 0x8000:
    switch (n)
    {
    case 0x0:
      set_vx_eq_vy(chirp, x, y);
      return;
    case 0x1:
      set_vx_eq_vx_or_vy(chirp, x, y);
      return;
    case 0x2:
      set_vx_eq_vx_and_vy(chirp, x, y);
      return;
    case 0x3:
      set_vx_eq_vx_xor_vy(chirp, x, y);
      return;
    case 0x4:
      set_vx_eq_vx_plus_vy(chirp, x, y);
      return;
    case 0x5:
      set_vx_eq_vx_minus_vy(chirp, x, y);
      return;
    case 0x6:
      if (chirp->config->shift_vx)
      {
        set_vx_eq_vx_shift_right(chirp, x);
      }
      else
      {
        set_vx_eq_vy_shift_right(chirp, x, y);
      }
      return;
    case 0x7:
      set_vx_eq_vy_minus_vx(chirp, x, y);
      return;
    case 0xE:
      if (chirp->config->shift_vx)
      {
        set_vx_eq_vx_shift_left(chirp, x);
      }
      else
      {
        set_vx_eq_vy_shift_left(chirp, x, y);
      }
      return;
    default:
      fprintf(stderr, "invalid instruction %04X\n", instruction);
      exit(1);
    }

  case 0x9000:
    if (n != 0x0)
    {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR, "invalid instruction %04X\n", instruction);
      exit(1);
    }
    skip_if_vx_neq_vy(chirp, x, y);
    return;

  case 0xA000:
    set_index_eq_nnn(chirp, nnn);
    return;

  case 0xB000:
    if (chirp->config->jump_with_nn)
    {
      jump_with_offset_nn(chirp, x, nn);
    }
    else
    {
      jump_with_offset_nnn(chirp, nnn);
    }
    return;

  case 0xC000:
    set_vx_eq_random(chirp, x, nn);
    return;

  case 0xD000:
    draw(chirp, x, y, n);
    return;

  case 0xE000:
    switch (nn)
    {
    case 0x9E:
      skip_if_key_vx_pressed(chirp, x);
      return;
    case 0xA1:
      skip_if_key_vx_not_pressed(chirp, x);
      return;
    default:
      fprintf(stderr, "invalid instruction %04X\n", instruction);
      exit(1);
    }

  case 0xF000:
    switch (nn)
    {
    case 0x07:
      set_vx_eq_delay(chirp, x);
      return;
    case 0x15:
      set_delay_eq_vx(chirp, x);
      return;
    case 0x18:
      set_sound_eq_vx(chirp, x);
      return;
    case 0x1E:
      set_index_eq_index_plus_vx(chirp, x);
      return;
    case 0x0A:
      get_key(chirp, x);
      return;
    case 0x29:
      set_index_eq_font(chirp, x);
      return;
    case 0x33:
      binary_coded_decimal_conversion(chirp, x);
      return;
    case 0x55:
      if (chirp->config->set_registers_increment_index)
      {
        set_registers_inc(chirp, x);
      }
      else
      {
        set_registers(chirp, x);
      }
      return;
    case 0x65:
      if (chirp->config->load_registers_increment_index)
      {
        load_registers_inc(chirp, x);
      }
      else
      {
        load_registers(chirp, x);
      }
      return;
    default:
      fprintf(stderr, "invalid instruction %04X\n", instruction);
      exit(1);
    }
  default:
    fprintf(stderr, "invalid instruction %04X\n", instruction);
    exit(1);
  }
}

void chirp_start_emulator_loop(Chirp* chirp, SDLWindow* window)
{
  const double timer_tick_interval = 1.0 / 60.0;
  const double cpu_tick_interval = 1.0 / ((float)chirp->config->cpu_speed);
  double timer_accumulator = 0.0;
  double cpu_accumulator = 0.0;

  uint64_t last_time = SDL_GetPerformanceCounter();
  const uint64_t freq = SDL_GetPerformanceFrequency();

  SDL_Event e;
  SDL_zero(e);

  while (chirp->is_running)
  {
    const uint64_t now = SDL_GetPerformanceCounter();
    const double dt = (double)(now - last_time) / (double)freq;

    timer_accumulator += dt;
    cpu_accumulator += dt;

    bool is_quitting = false;

    while (SDL_PollEvent(&e))
    {
      switch (e.type)
      {
      case SDL_EVENT_QUIT:
        chirp->is_running = false;
        is_quitting = true;
        break;
      case SDL_EVENT_KEY_DOWN:
        switch (e.key.key)
        {
        case SDLK_ESCAPE:
          chirp->is_running = false;
          is_quitting = true;
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
              chirp_registers_write(chirp->registers, chirp->waiting_key_register, i);
              chirp->is_waiting_for_key = false;
              chirp->waiting_key_register = -1;
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
      default:
        break;
      }
    }

    // update the timing
    last_time = now;

    if (chirp->is_paused || is_quitting)
    {
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
          const uint16_t instruction = chirp_fetch(chirp);

          // execute instruction
          chirp_execute(chirp, instruction);
        }
        cpu_accumulator -= cpu_tick_interval;
      }

      if (timer_accumulator >= timer_tick_interval)
      {
        // update timers
        chirp_update_timers(chirp, window);
        timer_accumulator -= timer_tick_interval;

        // render screen at 60Hz
        if (chirp->need_draw_screen)
        {
          sdl_window_draw_display(window, chirp->display);
          chirp->need_draw_screen = false;
        }
      }
    }
  }
}
