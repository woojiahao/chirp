# chirp

CHIP-8 emulator built with C!

Built this as an opportunity to also experiment with low-level programming.

## Getting started

I have only tested this with MacOS ARM64, but it should work with Linux.

Make sure to install SDL3 first ([instructions](https://github.com/libsdl-org/SDL/blob/main/INSTALL.md)).

> [!NOTE]
> I may migrate from Makefile to CMake in the future, but for now, chirp is primarily supported on MacOS!

```bash
git clone https://github.com/woojiahao/chirp.git
cd chirp
make clean
make all

out/chirp roms/ibm-logo.ch8
```

## Usage

Download the released binary for MacOS ARM64 and test:

```bash
usage: chirp ROM [options]
  [--debug]
  [--shift-vx]
  [--jump-with-vx]
  [--set-registers-increment-index]
  [--load-registers-increment-index]
  [--has-audio]
  [--cpu=N]
```

## Notes

As I was working on chirp, I was compiling my notes on Notion. These notes include CHIP-8 specification, instruction set
details, emulation techniques, SDL notes (graphics and audio programming), and rendering techniques.

[Notes here](https://woojiahao.notion.site/chirp-2f9f881eda0580158a73f6a072d7453b?source=copy_link)
