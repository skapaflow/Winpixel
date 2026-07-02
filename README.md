# Winpixel

Windows-only 2D graphics library in C, built as a DLL. Personal toolbox for rapid prototyping — not production-ready, just a fast way to get a window with drawing on screen.

> Work in progress. Expect rough edges and missing features.

## Requirements

- Windows
- [clang](https://releases.llvm.org/download.html)
- make (e.g. via [MSYS2](https://www.msys2.org/))

## Build

```sh
make
```

Produces `winpixel.dll` and `winpixel.lib` in `lib/`.

## Usage

```c
#include "winpixel.h"
```

Compile and link against the DLL:

```sh
clang your_program.c -o your_program.exe -I path/to/src -L path/to/lib -lwinpixel
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

- **Draw** — pixels, lines (solid, dashed, gap), rectangles, circles, ellipses, triangles, Bezier curves, thick splines; screen-door `_grid` variants for 50% transparency
- **Sprites** — load PNG via stb_image; draw with scale, flip, sub-region; save sprite or screenshot to PNG
- **Text** — built-in 6×6 bitmap font, printf-style, shadow, scale, measure
- **Vector font** — smooth-scaling vector text loaded at runtime from `.vtf` files (`wpx_font_load`, `wpx_text_font`, `wpx_text_font_center`); glyphs are baked into coverage bitmaps and cached per scale, so zooming stays cheap
- **Input** — keyboard (`VK_*`) and mouse (position, left/right/wheel, press/down/up states)
- **Math** — lerp, map, clamp, wrap, noise (1D Perlin), distance, direction, color interpolation, line intersection
- **Time** — FPS counter and delta time via `QueryPerformanceCounter`
- **Colors** — 30+ named constants (`RED`, `BLUE`, `BLUEPRINT`, …), `Color32` (hex RGBA), `Colorf` (float)
- **TimeGraph** — `wpx_timegraph` ring-buffer graph widget for real-time metrics

## Examples

Pre-built examples are in `examples/`. To compile them:

```sh
# Build the DLL first
make

# Then build examples
cd examples && make
```

| Example | Description |
|---------|-------------|
| `template.exe` | Minimal starting point |
| `triangle.exe` | Rotating triangle with mouse interaction and zoom |
| `sprite.exe` | Sprite loading and sub-region drawing (spritesheet) |
| `trails.exe` | Mouse trail effect with color interpolation |
| `noise.exe` | Scrolling terrain heightmap with Perlin noise and biome colors |
| `primitives.exe` | Gallery of all drawing primitives with labels |
| `bezier.exe` | Interactive Bezier curve editor (quadratic / cubic, draggable control points) |
| `paint.exe` | Mini paint app — draw, erase, color picker, screenshot |
| `clock.exe` | Analog clock reading system time with smooth hands |
| `timegraph.exe` | Live waveform display using `wpx_timegraph` (FPS, sin, Perlin noise) |
| `cube.exe` | Flat-shaded dithered cube with mouse-wheel zoom |
| `bunny.exe` | Stanford bunny — flat dither, dynamic mouse light, palette cycling |
| `font.exe` | Vector font (`.vtf`) with smooth mouse-wheel zoom |

## License

MIT — see [LICENSE](LICENSE).
