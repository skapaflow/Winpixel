# Winpixel

Windows-only 2D graphics library in C, built as a DLL. Personal toolbox for rapid prototyping — not production-ready, just a fast way to get a window with drawing on screen.

> Work in progress. Expect rough edges and missing features.

## Requirements

- Windows
- [clang](https://releases.llvm.org/download.html)
- make (e.g. via [MSYS2](https://www.msys2.org/))

## Build

```sh
cd src
make
```

Produces `winpixel.dll` and `winpixel.lib` in `src/`.

## Usage

```c
#include "winpixel.h"
```

Compile and link against the DLL:

```sh
clang your_program.c -o your_program.exe -I path/to/src -L path/to/src -lwinpixel
```

Make sure `winpixel.dll` is next to the executable (or on `PATH`) at runtime.

## Minimal example

```c
#include "winpixel.h"

int main() {
    winpixel_window("Hello", 800, 600, false);

    while (wpx_quit && !wpx_key_down(VK_ESCAPE)) {
        wpx_text_center(WHITE, 2, WPX_W2, WPX_H2, "Hello, World!");
        winpixel_render(BLACK, 1);  // present + clear + 1ms delay
    }

    return 0;
}
```

## Features

- **Draw** — pixels, lines, rectangles, circles, ellipses, triangles, Bezier curves, splines
- **Sprites** — load PNG/BMP via stb_image, draw with scale, flip, or sub-region
- **Text** — built-in 6×6 bitmap font, printf-style formatting
- **Input** — keyboard (VK_* codes) and mouse (position, buttons, wheel) with press/down/up states
- **Math** — lerp, map, clamp, wrap, noise (1D Perlin), distance, direction, color interpolation
- **Time** — FPS counter and delta time via `QueryPerformanceCounter`
- **Colors** — 30+ named constants (`RED`, `BLUE`, `BLUEPRINT`, …), `Color32` (hex), `Colorf` (float)

## Examples

Pre-built examples are in `examples/`. To compile them:

```sh
# Build the DLL first
cd src && make

# Then build examples
cd examples && make
```

| Example | Description |
|---------|-------------|
| `template.exe` | Minimal starting point |
| `triangle.exe` | Rotating triangle with mouse interaction and zoom |
| `sprite.exe` | Sprite loading and drawing |
| `trails.exe` | Mouse trail effect with color interpolation |

## License

MIT — see [LICENSE](LICENSE).
