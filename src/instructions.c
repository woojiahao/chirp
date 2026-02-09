#include <stdlib.h>

#include "instructions.h"
#include "display.h"

/**
 * Instruction: 00E0
 *
 * Clears the display on the next available processing.
 */
void clear_display(Chirp* chirp)
{
  if (chirp->config->is_debug)
  {
    SDL_Log("[00E0] clearing display\n");
  }

  chirp_display_clear(chirp->display);
  chirp->need_draw_screen = true;
}

/**
 * Instruction: DXYN
 *
 * Draws an N pixel tall sprite from memory location I, starting at (mem[VX], mem[VY]).
 *
 * Sets mem[VF] to 0, setting it to 1 if there are any pixels drawn.
 *
 * If the current pixel drawn is ON and the sprite's pixel is ON, set the current pixel drawn to OFF.
 */
void draw(Chirp* chirp, const int x, const int y, const uint8_t n)
{
  // starting position wraps around
  const uint8_t x_value = chirp_registers_read(chirp->registers, x) % DISPLAY_WIDTH;
  const uint8_t y_value = chirp_registers_read(chirp->registers, y) % DISPLAY_HEIGHT;

  // set VF = 0
  chirp_registers_write(chirp->registers, 0xF, 0);

  if (chirp->config->is_debug)
  {
    SDL_Log("[DXYN] drawing with position (%d, %d), with %d rows\n", x_value, y_value, n);
  }

  // draw row by row
  for (int dy = 0; dy < n; dy++)
  {
    const uint8_t pixel = chirp_mem_read(chirp->mem, chirp->index_register + dy);
    // no wrapping so just cut the image off
    if (y_value + dy >= DISPLAY_HEIGHT)
    {
      break;
    }

    // there's at most 8 columns worth of pixels
    for (int dx = 0; dx < 8; dx++)
    {
      if (x_value + dx >= DISPLAY_WIDTH)
      {
        break;
      }

      const uint8_t pixel_bit = pixel & 1 << (7 - dx);

      // if the sprite's pixel is 1, flip the existing pixel
      if (pixel_bit != 0)
      {
        const bool cur = chirp_display_get_pixel(chirp->display, x_value + dx, y_value + dy);
        if (cur)
        {
          chirp_registers_write(chirp->registers, 0xF, 1);
        }
        chirp_display_set_pixel(chirp->display, x_value + dx, y_value + dy, !cur);
      }
    }
  }

  chirp->need_draw_screen = true;
}

/**
 * Instruction: 00EE
 *
 * Returns from the current subroutine, popping the top-most instruction off the stack and setting the PC to it.
 */
void subroutine_return(Chirp* chirp)
{
  const uint16_t popped_addr = chirp_stack_pop(chirp->stack);
  if (chirp->config->is_debug)
  {
    SDL_Log("[00EE] returning from subroutine, back to %04X\n", popped_addr);
  }

  chirp->program_counter = popped_addr;
}

/**
 * Instruction: 2NNN
 *
 * Calls subroutine at address memory location NNN.
 *
 * Pushes current PC (after fetching the instruction) to the stack and then setting PC to NNN.
 */
void subroutine_call(Chirp* chirp, const uint16_t nnn)
{
  if (chirp->config->is_debug)
  {
    SDL_Log("[2NNN] calling subroutine at %04X, pushed %04X on stack\n", nnn, chirp->program_counter);
  }
  chirp_stack_push(chirp->stack, chirp->program_counter);
  chirp->program_counter = nnn;
}

/**
 * Instruction: 1NNN
 *
 * Jumps to memory address NNN.
 *
 * Sets PC to NNN.
 */
void jump(Chirp* chirp, const uint16_t nnn)
{
  if (chirp->config->is_debug)
  {
    SDL_Log("[1NNN] jumping to %04X, current program counter is %04X\n", nnn, chirp->program_counter);
  }
  chirp->program_counter = nnn;
}

/**
 * Instruction: BNNN (Variation 1)
 *
 * Jumps to memory address at mem[V0] + NNN.
 *
 * Variation 2 accessible via --jump-with-nn.
 */
void jump_with_offset_nnn(Chirp* chirp, const uint16_t nnn)
{
  const uint16_t destination = (uint16_t)chirp_registers_read(chirp->registers, 0x0) + nnn;
  if (chirp->config->is_debug)
  {
    SDL_Log(
      "[BNNN] jumping to %04X with offset of %d, current program counter is %04X\n",
      destination,
      nnn,
      chirp->program_counter);
  }
  chirp->program_counter = destination;
}

/**
 * Instruction: BNNN (Variation 2)
 *
 * Jumps to memory address at mem[VX] + NN.
 */
void jump_with_offset_nn(Chirp* chirp, const int x, const uint8_t nn)
{
  const uint16_t destination = (uint16_t)chirp_registers_read(chirp->registers, x) + nn;
  if (chirp->config->is_debug)
  {
    SDL_Log(
      "[BNNN] jumping to %04X with offset of %d, current program counter is %04X\n",
      destination,
      nn,
      chirp->program_counter);
  }
  chirp->program_counter = destination;
}

/**
 * Instruction: 3XNN
 *
 * Skips an instruction if mem[VX] == NN.
 */
void skip_if_vx_eq_nn(Chirp* chirp, const int x, const uint8_t nn)
{
  const uint8_t x_value = chirp_registers_read(chirp->registers, x);
  if (x_value == nn)
  {
    if (chirp->config->is_debug)
    {
      SDL_Log(
        "[3XNN] skipping instruction because %d == %d\n",
        x_value,
        nn
      );
    }
    chirp->program_counter += 2;
  }
  else if (chirp->config->is_debug)
  {
    SDL_Log(
      "[3XNN] NOT skipping instruction because %d != %d\n",
      x_value,
      nn
    );
  }
}

/**
 * Instruction: 4XNN
 *
 * Skips an instruction if mem[VX] != NN.
 */
void skip_if_vx_neq_nn(Chirp* chirp, const int x, const uint8_t nn)
{
  const uint8_t x_value = chirp_registers_read(chirp->registers, x);
  if (x_value != nn)
  {
    if (chirp->config->is_debug)
    {
      SDL_Log(
        "[4XNN] skipping instruction because %d != %d\n",
        x_value,
        nn
      );
    }
    chirp->program_counter += 2;
  }
  else if (chirp->config->is_debug)
  {
    SDL_Log(
      "[4XNN] NOT skipping instruction because %d == %d\n",
      x_value,
      nn
    );
  }
}

/**
 * Instruction: 5XY0
 *
 * Skips an instruction if mem[VX] == mem[VY].
 */
void skip_if_vx_eq_vy(Chirp* chirp, const int x, const int y)
{
  const uint8_t x_value = chirp_registers_read(chirp->registers, x);
  const uint8_t y_value = chirp_registers_read(chirp->registers, y);
  if (x_value == y_value)
  {
    if (chirp->config->is_debug)
    {
      SDL_Log(
        "[5XY0] skipping instruction because %d == %d\n",
        x_value,
        y_value
      );
    }
    chirp->program_counter += 2;
  }
  else if (chirp->config->is_debug)
  {
    SDL_Log(
      "[5XY0] NOT skipping instruction because %d != %d\n",
      x_value,
      y_value
    );
  }
}

/**
 * Instruction: 9XY0
 *
 * Skips an instruction if mem[VX] != mem[VY].
 */
void skip_if_vx_neq_vy(Chirp* chirp, const int x, const int y)
{
  const uint8_t x_value = chirp_registers_read(chirp->registers, x);
  const uint8_t y_value = chirp_registers_read(chirp->registers, y);

  if (x_value != y_value)
  {
    if (chirp->config->is_debug)
    {
      SDL_Log(
        "[9XY0] skipping instruction because %d != %d\n",
        x_value,
        y_value
      );
    }
    chirp->program_counter += 2;
  }
  else if (chirp->config->is_debug)
  {
    SDL_Log(
      "[9XY0] NOT skipping instruction because %d == %d\n",
      x_value,
      y_value
    );
  }
}

/**
 * Instruction: EX9E
 *
 * Skips an instruction if the key corresponding to mem[VX] is pressed.
 */
void skip_if_key_vx_pressed(Chirp* chirp, const int x)
{
  const uint8_t x_value = chirp_registers_read(chirp->registers, x);
  if (chirp_keyboard_read(chirp->keyboard, x_value))
  {
    if (chirp->config->is_debug)
    {
      SDL_Log(
        "[EX9E] skipping instruction because key %d is pressed\n",
        x_value
      );
    }
    chirp->program_counter += 2;
  }
  else if (chirp->config->is_debug)
  {
    SDL_Log(
      "[EX9E] NOT skipping instruction because key %d is NOT pressed\n",
      x_value
    );
  }
}

/**
 * Instruction: EXA1
 *
 * Skips an instruction if the key corresponding to mem[VX] is not pressed.
 */
void skip_if_key_vx_not_pressed(Chirp* chirp, const int x)
{
  const uint8_t x_value = chirp_registers_read(chirp->registers, x);
  if (!chirp_keyboard_read(chirp->keyboard, x_value))
  {
    if (chirp->config->is_debug)
    {
      SDL_Log(
        "[EXA1] skipping instruction because key %d is NOT pressed\n",
        x_value
      );
    }
    chirp->program_counter += 2;
  }
  else if (chirp->config->is_debug)
  {
    SDL_Log(
      "[EXA1] NOT skipping instruction because key %d is pressed\n",
      x_value
    );
  }
}

/**
 * Instruction: 6XNN
 *
 * Sets mem[VX] = NN.
 */
void set_vx_eq_nn(Chirp* chirp, const int x, const uint8_t nn)
{
  if (chirp->config->is_debug)
  {
    SDL_Log(
      "[6XNN] setting mem[V%d] = %d\n",
      x,
      nn
    );
  }
  chirp_registers_write(chirp->registers, x, nn);
}

/**
 * Instruction: 7XNN
 *
 * Sets mem[VX] = mem[VX] + NN.
 */
void set_vx_eq_vx_plus_nn(Chirp* chirp, const int x, const uint8_t nn)
{
  const uint8_t x_value = chirp_registers_read(chirp->registers, x);
  const uint8_t result = x_value + nn;

  if (chirp->config->is_debug)
  {
    SDL_Log(
      "[7XNN] setting mem[V%d] = mem[V%d] + %d (%d)\n",
      x,
      x,
      nn,
      result
    );
  }

  chirp_registers_write(chirp->registers, x, result);
}

/**
 * Instruction: 8XY0
 *
 * Sets mem[VX] = mem[VY].
 */
void set_vx_eq_vy(Chirp* chirp, const int x, const int y)
{
  const uint8_t y_value = chirp_registers_read(chirp->registers, y);

  if (chirp->config->is_debug)
  {
    SDL_Log(
      "[8XY0] setting mem[V%d] = mem[V%d] (%d)\n",
      x,
      y,
      y_value
    );
  }

  chirp_registers_write(chirp->registers, x, y_value);
}

/**
 * Instruction: 8XY1
 *
 * Sets mem[VX] = mem[VX] | mem[VY].
 */
void set_vx_eq_vx_or_vy(Chirp* chirp, const int x, const int y)
{
  const uint8_t x_value = chirp_registers_read(chirp->registers, x);
  const uint8_t y_value = chirp_registers_read(chirp->registers, y);
  const uint8_t result = x_value | y_value;
  if (chirp->config->is_debug)
  {
    SDL_Log(
      "[8XY1] setting mem[V%d] = mem[V%d] | mem[V%d] (%d)\n",
      x,
      x,
      y,
      result
    );
  }

  chirp_registers_write(chirp->registers, x, result);
}

/**
 * Instruction: 8XY2
 *
 * Sets mem[VX] = mem[VX] & mem[VY].
 */
void set_vx_eq_vx_and_vy(Chirp* chirp, const int x, const int y)
{
  const uint8_t x_value = chirp_registers_read(chirp->registers, x);
  const uint8_t y_value = chirp_registers_read(chirp->registers, y);
  const uint8_t result = x_value & y_value;

  if (chirp->config->is_debug)
  {
    SDL_Log(
      "[8XY2] setting mem[V%d] = mem[V%d] & mem[V%d] (%d)\n",
      x,
      x,
      y,
      result
    );
  }

  chirp_registers_write(chirp->registers, x, result);
}

/**
 * Instruction: 8XY3
 *
 * Sets mem[VX] = mem[VX] ^ mem[VY].
 */
void set_vx_eq_vx_xor_vy(Chirp* chirp, const int x, const int y)
{
  const uint8_t x_value = chirp_registers_read(chirp->registers, x);
  const uint8_t y_value = chirp_registers_read(chirp->registers, y);
  const uint8_t result = x_value ^ y_value;

  if (chirp->config->is_debug)
  {
    SDL_Log(
      "[8XY3] setting mem[V%d] = mem[V%d] ^ mem[V%d] (%d)\n",
      x,
      x,
      y,
      result
    );
  }

  chirp_registers_write(chirp->registers, x, result);
}

/**
 * Instruction: 8XY4
 *
 * Sets mem[VX] = mem[VX] + mem[VY].
 *
 * If the sum is greater than 255, set mem[VF] = 1, else, set to 0.
 */
void set_vx_eq_vx_plus_vy(Chirp* chirp, const int x, const int y)
{
  const uint8_t x_value = chirp_registers_read(chirp->registers, x);
  const uint8_t y_value = chirp_registers_read(chirp->registers, y);
  const uint16_t sum = x_value + y_value;
  bool has_overflow = false;

  if (sum > 255)
  {
    has_overflow = true;
    chirp_registers_write(chirp->registers, 0xF, 1);
  }
  else
  {
    chirp_registers_write(chirp->registers, 0xF, 0);
  }

  const uint8_t result = sum & 0xFF;

  if (chirp->config->is_debug)
  {
    SDL_Log(
      "[8XY4] setting mem[V%d] = mem[V%d] + mem[V%d] %s (%d)\n",
      x,
      x,
      y,
      has_overflow
        ? "with overflow"
        : "without overflow",
      result
    );
  }

  chirp_registers_write(chirp->registers, x, result);
}

/**
 * Instruction: 8XY5
 *
 * Sets mem[VX] = mem[VX] - mem[VY].
 *
 * If mem[VX] > mem[VY], then set mem[VF] = 1, else, set to 0.
 */
void set_vx_eq_vx_minus_vy(Chirp* chirp, const int x, const int y)
{
  const uint8_t x_value = chirp_registers_read(chirp->registers, x);
  const uint8_t y_value = chirp_registers_read(chirp->registers, y);
  bool has_underflow = 0;

  if (x_value > y_value)
  {
    has_underflow = true;
    chirp_registers_write(chirp->registers, 0xF, 1);
  }
  else
  {
    chirp_registers_write(chirp->registers, 0xF, 0);
  }
  const uint8_t result = x_value - y_value;

  if (chirp->config->is_debug)
  {
    SDL_Log(
      "[8XY5] setting mem[V%d] = mem[V%d] - mem[V%d] %s (%d)\n",
      x,
      x,
      y,
      has_underflow
        ? "with underflow"
        : "without underflow",
      result
    );
  }

  chirp_registers_write(chirp->registers, x, result);
}

/**
 * Instruction: 8XY6 (Variation 1)
 *
 * Sets mem[VX] = mem[VY] >> 1.
 *
 * If the shifted bit was 1, set mem[VF] = 1, else, set to 0.
 *
 * Variation 2 accessible via --shift-vx.
 */
void set_vx_eq_vy_shift_right(Chirp* chirp, const int x, const int y)
{
  const uint8_t y_value = chirp_registers_read(chirp->registers, y);
  const uint8_t shifted_bit = y_value & 1;
  const bool has_carry = shifted_bit != 0;

  chirp_registers_write(chirp->registers, 0xF, has_carry ? 1 : 0);

  const uint8_t result = y_value >> 1;

  if (chirp->config->is_debug)
  {
    SDL_Log(
      "[8XY6] setting mem[V%d] = mem[V%d] >> 1 %s (%d)\n",
      x,
      y,
      has_carry
        ? "with carry"
        : "without carry",
      result
    );
  }

  chirp_registers_write(chirp->registers, x, result);
}

/**
 * Instruction: 8XY6 (Variation 2)
 *
 * Sets mem[VX] = mem[VX] >> 1.
 *
 * If the shifted bit was 1, set mem[VF] = 1, else, set to 0.
 */
void set_vx_eq_vx_shift_right(Chirp* chirp, const int x)
{
  const uint8_t x_value = chirp_registers_read(chirp->registers, x);
  const uint8_t shifted_bit = x_value & 1;
  const bool has_carry = shifted_bit != 0;

  chirp_registers_write(chirp->registers, 0xF, has_carry ? 1 : 0);

  const uint8_t result = x_value >> 1;

  if (chirp->config->is_debug)
  {
    SDL_Log(
      "[8XY6] setting mem[V%d] = mem[V%d] >> 1 %s (%d)\n",
      x,
      x,
      has_carry
        ? "with carry"
        : "without carry",
      result
    );
  }

  chirp_registers_write(chirp->registers, x, result);
}

/**
 * Instruction: 8XY7
 *
 * Sets mem[VX] = mem[VY] - mem[VX].
 *
 * If mem[VY] > mem[VX], then set mem[VF] = 1, else, set to 0.
 */
void set_vx_eq_vy_minus_vx(Chirp* chirp, const int x, const int y)
{
  const uint8_t x_value = chirp_registers_read(chirp->registers, x);
  const uint8_t y_value = chirp_registers_read(chirp->registers, y);
  const bool has_underflow = y_value > x_value;

  chirp_registers_write(chirp->registers, 0xF, has_underflow ? 1 : 0);

  const uint8_t result = y_value - x_value;

  if (chirp->config->is_debug)
  {
    SDL_Log(
      "[8XY7] setting mem[V%d] = mem[V%d] - mem[V%d] %s (%d)\n",
      x,
      y,
      x,
      has_underflow
        ? "with underflow"
        : "without underflow",
      result
    );
  }

  chirp_registers_write(chirp->registers, x, result);
}

/**
 * Instruction: 8XYE (Variation 1)
 *
 * Sets mem[VX] = mem[VY] << 1.
 *
 * If the shifted bit was 1, set mem[VF] = 1, else, set to 0.
 *
 * Variation 2 accessible via --shift-vx.
 */
void set_vx_eq_vy_shift_left(Chirp* chirp, const int x, const int y)
{
  const uint8_t y_value = chirp_registers_read(chirp->registers, y);
  const uint8_t shifted_bit = y_value & 0x80;
  const bool has_carry = shifted_bit != 0;

  chirp_registers_write(chirp->registers, 0xF, has_carry ? 1 : 0);

  const uint8_t result = y_value << 1;

  if (chirp->config->is_debug)
  {
    SDL_Log(
      "[8XYE] setting mem[V%d] = mem[V%d] << 1 %s (%d)\n",
      x,
      y,
      has_carry
        ? "with carry"
        : "without carry",
      result
    );
  }

  chirp_registers_write(chirp->registers, x, result);
}

/**
 * Instruction: 8XYE (Variation 2)
 *
 * Sets mem[VX] = mem[VY] << 1.
 *
 * If the shifted bit was 1, set mem[VF] = 1, else, set to 0.
 */
void set_vx_eq_vx_shift_left(Chirp* chirp, const int x)
{
  const uint8_t x_value = chirp_registers_read(chirp->registers, x);
  const uint8_t shifted_bit = x_value & 0x80;
  const bool has_carry = shifted_bit != 0;

  chirp_registers_write(chirp->registers, 0xF, has_carry ? 1 : 0);

  const uint8_t result = x_value << 1;

  if (chirp->config->is_debug)
  {
    SDL_Log(
      "[8XYE] setting mem[V%d] = mem[V%d] << 1 %s (%d)\n",
      x,
      x,
      has_carry
        ? "with carry"
        : "without carry",
      result
    );
  }

  chirp_registers_write(chirp->registers, x, result);
}

/**
 * Instruction: CXNN
 *
 * Sets mem[VX] = random() & NN where random number is between 0 and 255.
 */
void set_vx_eq_random(Chirp* chirp, const int x, const uint8_t nn)
{
  const uint8_t random = rand() % 256;
  const uint8_t result = random & nn;

  if (chirp->config->is_debug)
  {
    SDL_Log(
      "[CXNN] setting mem[V%d] = %d && %d (%d)\n",
      x,
      random,
      nn,
      result
    );
  }

  chirp_registers_write(chirp->registers, x, result);
}

/**
 * Instruction: FX07
 *
 * Sets mem[VX] = delay timer.
 */
void set_vx_eq_delay(Chirp* chirp, const int x)
{
  const uint8_t delay = chirp->delay_timer;

  if (chirp->config->is_debug)
  {
    SDL_Log(
      "[FX07] setting mem[V%d] = %d \n",
      x,
      delay
    );
  }

  chirp_registers_write(chirp->registers, x, delay);
}

/**
 * Instruction: ANNN
 *
 * Sets index to NNN.
 */
void set_index_eq_nnn(Chirp* chirp, const uint16_t nnn)
{
  if (chirp->config->is_debug)
  {
    SDL_Log("[ANNN] setting I = %d\n", nnn);
  }

  chirp->index_register = nnn;
}

/**
 * Instruction: FX1E
 *
 * Adds mem[VX] to index.
 */
void set_index_eq_index_plus_vx(Chirp* chirp, const int x)
{
  const uint8_t x_value = chirp_registers_read(chirp->registers, x);
  const uint16_t result = chirp->index_register + x_value;

  if (chirp->config->is_debug)
  {
    SDL_Log("[FX1E] setting I = I + mem[V%d] (%d)\n", x, result);
  }

  chirp->index_register = result;
}

/**
 * Instruction: FX29
 *
 * Sets index to a font character in memory.
 */
void set_index_eq_font(Chirp* chirp, const int x)
{
  const uint8_t x_value = chirp_registers_read(chirp->registers, x);
  const uint16_t hex = CHIRP_FONTS_ADDR_START + x_value * 0x5;

  if (chirp->config->is_debug)
  {
    SDL_Log("[FX29] setting I = %d (font character)\n", hex);
  }

  chirp->index_register = hex;
}

/**
 * Instruction: FX55 (Variation 1)
 *
 * Sets value of registers V0 to VX (i.e. mem[V0] to mem[VX]) stored into successive memory addresses starting at I.
 *
 * Variation 2 accessible via --set-registers-increment-index.
 */
void set_registers(Chirp* chirp, const int x)
{
  for (int i = 0; i <= x; i++)
  {
    const uint8_t value = chirp_registers_read(chirp->registers, i);
    chirp_mem_write(chirp->mem, chirp->index_register + i, value);

    if (chirp->config->is_debug)
    {
      SDL_Log("[FX55] setting (I + %d) = %d\n", i, value);
    }
  }
}

/**
 * Instruction: FX55 (Variation 2)
 *
 * Sets value of registers V0 to VX (i.e. mem[V0] to mem[VX]) stored into successive memory addresses starting at I.
 *
 * I is incremented with each set.
 */
void set_registers_inc(Chirp* chirp, const int x)
{
  for (int i = 0; i <= x; i++)
  {
    const uint8_t value = chirp_registers_read(chirp->registers, i);
    chirp_mem_write(chirp->mem, chirp->index_register++, value);

    if (chirp->config->is_debug)
    {
      SDL_Log("[FX55] setting (I + %d) = %d\n", i, value);
    }
  }
}

/**
 * Instruction: FX65 (Variation 1)
 *
 * Loads memory addresses into registers V0 to VX (i.e. mem[V0] to mem[VX]) starting at I.
 *
 * Variation 2 accessible via --load-registers-increment-index.
 */
void load_registers(Chirp* chirp, const int x)
{
  for (int i = 0; i <= x; i++)
  {
    const uint8_t value = chirp_mem_read(chirp->mem, chirp->index_register + i);
    chirp_registers_write(chirp->registers, i, value);

    if (chirp->config->is_debug)
    {
      SDL_Log("[FX65] setting mem[V%d] = %d\n", i, value);
    }
  }
}

/**
 * Instruction: FX65 (Variation 2)
 *
 * Loads memory addresses into registers V0 to VX (i.e. mem[V0] to mem[VX]) starting at I.
 *
 * I is incremented with each load.
 */
void load_registers_inc(Chirp* chirp, const int x)
{
  for (int i = 0; i <= x; i++)
  {
    const uint8_t value = chirp_mem_read(chirp->mem, chirp->index_register++);
    chirp_registers_write(chirp->registers, i, value);

    if (chirp->config->is_debug)
    {
      SDL_Log("[FX65] setting mem[V%d] = %d\n", i, value);
    }
  }
}

/**
 * Instruction: FX15
 *
 * Sets delay timer equivalent to mem[VX].
 */
void set_delay_eq_vx(Chirp* chirp, const int x)
{
  const uint8_t x_value = chirp_registers_read(chirp->registers, x);
  chirp->delay_timer = x_value;

  if (chirp->config->is_debug)
  {
    SDL_Log("[FX15] setting delay timer = mem[V%d] (%d)\n", x, x_value);
  }
}

/**
 * Instruction: FX18
 *
 * Sets sound timer equivalent to mem[VX].
 */
void set_sound_eq_vx(Chirp* chirp, const int x)
{
  const uint8_t x_value = chirp_registers_read(chirp->registers, x);
  chirp->sound_timer = x_value;

  if (chirp->config->is_debug)
  {
    SDL_Log("[FX18] setting sound timer = mem[V%d] (%d)\n", x, x_value);
  }
}

/**
 * Instruction: FX0A
 *
 * Blocks CPU processing until any key is pressed and store into VX.
 */
void get_key(Chirp* chirp, const int x)
{
  chirp->is_waiting_for_key = true;
  chirp->waiting_key_register = x;
  // rest of implementation set in emulator loop on user input

  if (chirp->config->is_debug)
  {
    SDL_Log("[FX0A] waiting for key %d to be pressed\n", x);
  }
}

/**
 * Instruction: FX33
 *
 * Given mem[VX], extract the digits and store into mem[I], mem[I + 1], mem[I + 2].
 */
void binary_coded_decimal_conversion(Chirp* chirp, const int x)
{
  uint8_t x_value = chirp_registers_read(chirp->registers, x);

  for (int i = 0; i < 3; i++)
  {
    // mem[I + 2 - i] = digit popped
    const uint8_t digit = x_value % 10;
    chirp_mem_write(chirp->mem, chirp->index_register + 2 - i, digit);
    x_value = x_value / 10;

    if (chirp->config->is_debug)
    {
      SDL_Log("[FX33] setting mem[I + %d] = %d\n", chirp->index_register + 2 - i, digit);
    }
  }
}
