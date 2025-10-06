# Simple cross-platform Makefile for reseau-socket-C
# Detect platform: macOS/Linux vs Windows (MinGW/MSYS)

OSNAME := $(shell uname -s 2>/dev/null || echo Unknown)

# Default tools
CC := gcc
AR := ar

# Source and output paths
SRV_DIR := Serveur
CLI_DIR := Client

SRV_SRC := $(SRV_DIR)/server2.c $(SRV_DIR)/history.c
CLI_SRC := $(CLI_DIR)/client2.c

SRV_BIN := $(SRV_DIR)/serveur
CLI_BIN := $(CLI_DIR)/client

# Platform-specific flags and libs
CFLAGS_COMMON := -Wall -Wextra -O2
LDFLAGS_COMMON :=

ifeq ($(OSNAME),Darwin)
# macOS uses POSIX sockets
  CFLAGS := $(CFLAGS_COMMON)
  LDLIBS :=
else ifeq ($(OSNAME),Linux)
# Linux uses POSIX sockets
  CFLAGS := $(CFLAGS_COMMON)
  LDLIBS :=
else
# Assume Windows (MinGW/MSYS). If this is wrong, override by passing OSNAME=Darwin/Linux
  CFLAGS := $(CFLAGS_COMMON) -DWIN32
  LDLIBS := -lws2_32
endif

.PHONY: all server client clean distclean

all: server client

server: $(SRV_BIN)
client: $(CLI_BIN)

$(SRV_BIN): $(SRV_SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS_COMMON) $(LDLIBS)

$(CLI_BIN): $(CLI_SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS_COMMON) $(LDLIBS)

clean:
	rm -f $(SRV_BIN) $(CLI_BIN)

distclean: clean
	@true

# Usage examples:
#   make            # builds both
#   make server     # builds server only
#   make client     # builds client only
#   make clean      # removes binaries
# Force platform override if needed:
#   make OSNAME=Darwin
#   make OSNAME=Linux

