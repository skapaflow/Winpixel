SHELL   = cmd.exe

CC      = clang
CFLAGS  = -Wall -Wextra -s -O2 -I"../src"
LDFLAGS = -L"../lib" -lwinpixel

NAMES   = sprite trails triangle template
TARGETS = $(NAMES:=.exe)

all: $(TARGETS)

%.exe: %.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

clean:
	del /Q $(TARGETS) 2>nul || exit 0