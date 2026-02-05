CC      = gcc
CFLAGS  = -Wall -Wextra -Werror -std=c11
CFLAGS += -Wno-unused-parameter
LDFLAGS =

# For MacOS linking to SDL3
CFLAGS += -I/usr/local/include/SDL3
LDFLAGS += -L/usr/local/lib -lSDL3
LDFLAGS += -Wl,-rpath,/usr/local/lib

OUT_DIR = out
BIN     = chirp
SRC_DIR = src

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c,$(OUT_DIR)/%.o,$(SRC))

CFLAGS += -MMD -MP
DEPS = $(OBJ:.o=.d)

.PHONY: all clean

all: $(OUT_DIR)/$(BIN)

# Link
$(OUT_DIR)/$(BIN): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

# Compile
$(OUT_DIR)/%.o: $(SRC_DIR)/%.c | $(OUT_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Ensure out/ exists
$(OUT_DIR):
	mkdir -p $(OUT_DIR)

-include $(DEPS)

clean:
	rm -rf $(OUT_DIR)
