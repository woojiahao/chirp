# chirp

CHIP-8 emulator built with C

## Specifications

CHIP-8 has the following components, which have to all be implemented in this `chirp`:

- Memory: direct access to up to 4KB of RAM
- Display: 64 by 32 pixels; monochrome
- Program counter (`PC`): points to the current instruction in memory
- 16-bit index register (`I`): point at locations in memory
- Stack for 16-bit addresses: used to call subroutines/functions and return from them
- 8-bit delay timer: decremented at a rate of 60 times per second until it reaches 0
- 8-bit sound timer: similar to delay timer but it gives off a beeping sound as long as it's not 0
- 16 8-bit general-purpose variable registers number `0` to `F`
  - `VF`: flag register

## Process

### Implementing memory

We need to start by literally simulating the RAM that we are going to be able to operate on.

Creating an abstraction over the memory rather than literally trying to treat it as some blocks of memory (credits to Lazy Stripes).

In C, this can be achieved by using 

## References

- [Guide to making a CHIP-8 emulator](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/) by Tobias V. I. Langhoff
- [Writing your first emulator](https://gecko05.github.io/2022/10/22/first-emulator-part1.html) by Jaime Centeno
- [Writing an emulator: memory management](https://blog.tigris.fr/2019/07/28/writing-an-emulator-memory-management/) by Lazy Stripes