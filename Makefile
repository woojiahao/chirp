CC      ?= gcc
OUT_DIR := out
BIN     := chirp
SRC_DIR := src

SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(patsubst $(SRC_DIR)/%.c,$(OUT_DIR)/%.o,$(SRC))
DEPS := $(OBJ:.o=.d)

# Base flags
CFLAGS  := -Wall -Wextra -Werror -std=c11 -Wno-unused-parameter
CFLAGS  += -MMD -MP
LDFLAGS :=

# SDL3 flags (portable)
SDL_CFLAGS  := $(shell pkg-config --cflags sdl3 2>/dev/null)
SDL_LDFLAGS := $(shell pkg-config --libs sdl3 2>/dev/null)

# Fail early if SDL3 isn't found
ifeq ($(strip $(SDL_CFLAGS)),)
$(error SDL3 not found. Install SDL3 development files and ensure pkg-config can find sdl3)
endif

CFLAGS  += $(SDL_CFLAGS)
LDFLAGS += $(SDL_LDFLAGS)

.PHONY: all clean

all: $(OUT_DIR)/$(BIN)

$(OUT_DIR)/$(BIN): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

$(OUT_DIR)/%.o: $(SRC_DIR)/%.c | $(OUT_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT_DIR):
	mkdir -p $(OUT_DIR)

-include $(DEPS)

clean:
	rm -rf $(OUT_DIR)
