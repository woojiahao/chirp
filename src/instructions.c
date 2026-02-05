#include "instructions.h"
#include "display.h"
#include <stdlib.h>

void clear_display(Chirp *chirp)
{
  // 00E0
  chirp_display_clear(chirp->display);
  if (chirp->is_debug)
  {
    SDL_Log("clearing display\n");
  }
  chirp->need_draw_screen = true;
}

void draw(Chirp *chirp, int vx, int vy, uint8_t n)
{
  // DXYN

  // starting position wraps around
  uint8_t x = chirp_registers_read(chirp->registers, vx) % DISPLAY_WIDTH;
  uint8_t y = chirp_registers_read(chirp->registers, vy) % DISPLAY_HEIGHT;

  // set VF = 0
  chirp_registers_write(chirp->registers, 0xF, 0);

  if (chirp->is_debug)
  {
    SDL_Log("drawing with position (%d, %d), with %d rows\n", x, y, n);
  }

  // draw row by row
  for (int y_v = 0; y_v < n; y_v++)
  {
    uint8_t pixel = chirp_mem_read(chirp->mem, chirp->index_register + y_v);
    if (y + y_v >= DISPLAY_HEIGHT)
    {
      break;
    }

    for (int x_v = 0; x_v < 8; x_v++)
    {
      if (x + x_v >= DISPLAY_WIDTH)
      {
        break;
      }
      uint8_t pixel_bit = pixel & (1 << (7 - x_v));
      if (pixel_bit != 0)
      {
        if (chirp_display_get_pixel(chirp->display, x + x_v, y + y_v))
        {
          chirp_registers_write(chirp->registers, 0xF, 1);
          chirp_display_set_pixel(chirp->display, x + x_v, y + y_v, false);
        }
        else
        {
          chirp_display_set_pixel(chirp->display, x + x_v, y + y_v, true);
        }
      }
      else
      {
        chirp_display_set_pixel(chirp->display, x + x_v, y + y_v, false);
      }
    }
  }

  chirp->need_draw_screen = true;
}

void subroutine_return(Chirp *chirp)
{
  // 00EE

  uint16_t popped_addr = chirp_stack_pop(chirp->stack);
  if (chirp->is_debug)
  {
    SDL_Log("returning from subroutine, back to %04X\n", popped_addr);
  }
  chirp->program_counter = popped_addr;
}

void subroutine_call(Chirp *chirp, uint16_t nnn)
{
  // 2NNN

  if (chirp->is_debug)
  {
    SDL_Log("calling subroutine at %04X, pushed %04X on stack\n", nnn, chirp->program_counter);
  }
  chirp_stack_push(chirp->stack, chirp->program_counter);
  chirp->program_counter = nnn;
}

void jump(Chirp *chirp, uint16_t nnn)
{
  // 1NNN

  if (chirp->is_debug)
  {
    SDL_Log("jumping to %04X, current program counter is %04X\n", nnn, chirp->program_counter);
  }
  chirp->program_counter = nnn;
}

void jump_with_offset(Chirp *chirp, uint16_t nnn)
{
  // TODO: Make this configurable with VX potentially
  // BNNN

  uint16_t destination = (uint16_t)chirp_registers_read(chirp->registers, 0x0) + nnn;
  if (chirp->is_debug)
  {
    SDL_Log(
        "jumping to %04X with offset of %d, current program counter is %04X\n",
        destination,
        nnn,
        chirp->program_counter);
  }
  chirp->program_counter = destination;
}

void skip_if_vx_eq_nn(Chirp *chirp, int vx, uint8_t nn)
{
  // 3XNN
  if (chirp_registers_read(chirp->registers, vx) == nn)
  {
    chirp->program_counter += 2;
  }
}

void skip_if_vx_neq_nn(Chirp *chirp, int vx, uint8_t nn)
{
  // 4XNN
  if (chirp_registers_read(chirp->registers, vx) != nn)
  {
    chirp->program_counter += 2;
  }
}

void skip_if_vx_eq_vy(Chirp *chirp, int vx, int vy)
{
  // 5XY0
  if (chirp_registers_read(chirp->registers, vx) == chirp_registers_read(chirp->registers, vy))
  {
    chirp->program_counter += 2;
  }
}

void skip_if_vx_neq_vy(Chirp *chirp, int vx, int vy)
{
  // 9XY0
  if (chirp_registers_read(chirp->registers, vx) != chirp_registers_read(chirp->registers, vy))
  {
    chirp->program_counter += 2;
  }
}

void skip_if_key_eq_vx(Chirp *chirp, int vx)
{
  // EX9E
  uint8_t vx_value = chirp_registers_read(chirp->registers, vx);
  if (chirp_keyboard_read(chirp->keyboard, vx_value))
  {
    chirp->program_counter += 2;
  }
}

void skip_if_key_neq_vx(Chirp *chirp, int vx)
{
  // EX9E
  uint8_t vx_value = chirp_registers_read(chirp->registers, vx);
  if (!chirp_keyboard_read(chirp->keyboard, vx_value))
  {
    chirp->program_counter += 2;
  }
}

void set_vx_eq_nn(Chirp *chirp, int vx, uint8_t nn)
{
  // 6XNN

  chirp_registers_write(chirp->registers, vx, nn);
}

void set_vx_eq_vx_plus_nn(Chirp *chirp, int vx, uint8_t nn)
{
  // 7XNN

  uint8_t vx_value = chirp_registers_read(chirp->registers, vx);
  chirp_registers_write(chirp->registers, vx, vx_value + nn);
}

void set_vx_to_vy(Chirp *chirp, int vx, int vy)
{
  // 8XY0
  uint8_t vy_value = chirp_registers_read(chirp->registers, vy);
  chirp_registers_write(chirp->registers, vx, vy_value);
}

void set_vx_eq_vx_or_vy(Chirp *chirp, int vx, int vy)
{
  // 8XY1
  uint8_t vx_value = chirp_registers_read(chirp->registers, vx);
  uint8_t vy_value = chirp_registers_read(chirp->registers, vy);
  chirp_registers_write(chirp->registers, vx, vx_value | vy_value);
}

void set_vx_eq_vx_and_vy(Chirp *chirp, int vx, int vy)
{
  // 8XY2
  uint8_t vx_value = chirp_registers_read(chirp->registers, vx);
  uint8_t vy_value = chirp_registers_read(chirp->registers, vy);
  chirp_registers_write(chirp->registers, vx, vx_value & vy_value);
}

void set_vx_eq_vx_xor_vy(Chirp *chirp, int vx, int vy)
{
  // 8XY3
  uint8_t vx_value = chirp_registers_read(chirp->registers, vx);
  uint8_t vy_value = chirp_registers_read(chirp->registers, vy);
  chirp_registers_write(chirp->registers, vx, vx_value ^ vy_value);
}

void set_vx_eq_vx_plus_vy(Chirp *chirp, int vx, int vy)
{
  // 8XY4
  uint8_t vx_value = chirp_registers_read(chirp->registers, vx);
  uint8_t vy_value = chirp_registers_read(chirp->registers, vy);
  uint16_t sum = vx_value + vy_value;

  if (sum > 255)
  {
    chirp_registers_write(chirp->registers, 0xF, 1);
  }
  else
  {
    chirp_registers_write(chirp->registers, 0xF, 0);
  }

  chirp_registers_write(chirp->registers, vx, sum & 0xFF);
}

void set_vx_eq_vx_minus_vy(Chirp *chirp, int vx, int vy)
{
  // 8XY5
  uint8_t vx_value = chirp_registers_read(chirp->registers, vx);
  uint8_t vy_value = chirp_registers_read(chirp->registers, vy);
  if (vx_value > vy_value)
  {
    chirp_registers_write(chirp->registers, 0xF, 1);
  }
  else
  {
    chirp_registers_write(chirp->registers, 0xF, 0);
  }
  chirp_registers_write(chirp->registers, vx, vx_value - vy_value);
}

void set_vx_eq_vy_shift_right(Chirp *chirp, int vx, int vy)
{
  // 8XY6
  uint8_t vy_value = chirp_registers_read(chirp->registers, vy);
  uint8_t shifted_bit = vy_value & 1;
  uint8_t new_vx_value = vy_value >> 1;
  if (shifted_bit != 0)
  {
    chirp_registers_write(chirp->registers, 0xF, 1);
  }
  else
  {
    chirp_registers_write(chirp->registers, 0xF, 0);
  }
  chirp_registers_write(chirp->registers, vx, new_vx_value);
}

void set_vx_eq_vy_minus_vx(Chirp *chirp, int vx, int vy)
{
  // 8XY7
  uint8_t vx_value = chirp_registers_read(chirp->registers, vx);
  uint8_t vy_value = chirp_registers_read(chirp->registers, vy);
  if (vy_value > vx_value)
  {
    chirp_registers_write(chirp->registers, 0xF, 1);
  }
  else
  {
    chirp_registers_write(chirp->registers, 0xF, 0);
  }
  chirp_registers_write(chirp->registers, vx, vy_value - vx_value);
}

void set_vx_eq_vy_shift_left(Chirp *chirp, int vx, int vy)
{
  // 8XYE
  uint8_t vy_value = chirp_registers_read(chirp->registers, vy);
  uint8_t shifted_bit = vy_value & 0x80;
  uint8_t new_vx_value = vy_value << 1;
  if (shifted_bit != 0)
  {
    chirp_registers_write(chirp->registers, 0xF, 1);
  }
  else
  {
    chirp_registers_write(chirp->registers, 0xF, 0);
  }
  chirp_registers_write(chirp->registers, vx, new_vx_value);
}

void set_vx_eq_random(Chirp *chirp, int vx, uint8_t nn)
{
  // CXNN
  uint8_t random = rand() % 256;
  chirp_registers_write(chirp->registers, vx, random & nn);
}

void set_vx_eq_delay(Chirp *chirp, int vx)
{
  // FX07
  uint8_t delay = chirp->delay_timer;
  chirp_registers_write(chirp->registers, vx, delay);
}

void set_index_eq_nnn(Chirp *chirp, uint16_t nnn)
{
  // ANNN

  chirp->index_register = nnn;
}

void set_index_eq_index_plus_vx(Chirp *chirp, int vx)
{
  // FX1E
  uint8_t vx_value = chirp_registers_read(chirp->registers, vx);
  chirp->index_register += vx_value;
}

void set_index_to_font(Chirp *chirp, int vx)
{
  // FX29
  uint8_t vx_value = chirp_registers_read(chirp->registers, vx);
  uint8_t hex = CHIRP_FONTS_ADDR_START + vx_value * 0x5;
  chirp->index_register = hex;
}

void set_registers(Chirp *chirp, int vx)
{
  // FX55
  for (int i = 0; i <= vx; i++)
  {
    uint8_t value = chirp_registers_read(chirp->registers, i);
    chirp_mem_write(chirp->mem, chirp->index_register + i, value);
  }
}

void load_registers(Chirp *chirp, int vx)
{
  // FX65
  for (int i = 0; i <= vx; i++)
  {
    uint8_t value = chirp_mem_read(chirp->mem, chirp->index_register + i);
    chirp_registers_write(chirp->registers, i, value);
  }
}

void get_key(Chirp *chirp, int vx)
{
  // FX0A
  chirp->is_waiting_for_key = true;
  // rest of implementation set in emulator loop on user input
}

void set_delay_eq_vx(Chirp *chirp, int vx)
{
  // FX15
  uint8_t vx_value = chirp_registers_read(chirp->registers, vx);
  chirp->delay_timer = vx_value;
}

void set_sound_eq_vx(Chirp *chirp, int vx)
{
  // FX18
  uint8_t vx_value = chirp_registers_read(chirp->registers, vx);
  chirp->sound_timer = vx_value;
}

void binary_coded_decimal_conversion(Chirp *chirp, int vx)
{
  // FX33
  uint8_t vx_value = chirp_registers_read(chirp->registers, vx);
  for (int i = 0; i < 3; i++)
  {
    // mem[I + 2 - i] = digit popped
    uint8_t digit = (int)(vx_value % 10);
    chirp_mem_write(chirp->mem, chirp->index_register + 2 - i, digit);
    vx_value = (int)(vx_value / 10);
  }
}