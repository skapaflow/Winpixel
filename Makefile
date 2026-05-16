SHELL = cmd.exe

CC      = clang
CFLAGS  = -Wall -Wextra -O2 -I src -D WINPIXEL_DLL
LDFLAGS = -shared -s -Wl,--out-implib,lib/winpixel.lib -lgdi32 -luser32 -lcomdlg32 -lshell32

SRCS = src/winpixel.c src/input.c src/draw.c src/time.c src/print.c src/sprite.c src/math.c
OBJS = $(SRCS:src/%.c=lib/obj/%.o)

all: lib/winpixel.dll

lib/winpixel.dll: $(OBJS)
	$(CC) $(OBJS) -o lib/winpixel.dll $(LDFLAGS)

lib/obj/%.o: src/%.c src/winpixel.h src/winpixel_internal.h
	if not exist lib\obj mkdir lib\obj
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del /Q lib\winpixel.dll lib\winpixel.lib lib\obj\*.o 2>nul || exit 0
