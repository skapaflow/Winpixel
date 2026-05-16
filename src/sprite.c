#include "winpixel_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define WRGB(r,g,b)     (((uint32_t)((r) & 0xFF) << 24) | ((uint32_t)((g) & 0xFF) << 16) | ((uint32_t)((b) & 0xFF) << 8) | 0xFF)
#define WRGBA(r,g,b,a)  (((uint32_t)((r) & 0xFF) << 24) | ((uint32_t)((g) & 0xFF) << 16) | ((uint32_t)((b) & 0xFF) << 8) | ((uint32_t)((a) & 0xFF)))

/* color key: skip pixel if it matches MAGENTA (ignores alpha) */
#define _WPX_IS_COLORKEY(c) (((c) & 0xFFFFFF00u) == (MAGENTA & 0xFFFFFF00u))

/* ---------------------------------------------------------------
 * Sprite create / free
 * --------------------------------------------------------------- */

WINPIXELDLL WPX_Sprite *WINPIXELCALL wpx_sprite_create (int w, int h) {

    WPX_Sprite *s = calloc(1, sizeof(WPX_Sprite));
    if (!s) return NULL;
    s->w      = w;
    s->h      = h;
    s->pixels = calloc(w * h, sizeof(uint32_t));
    if (!s->pixels) { free(s); return NULL; }
    return s;
}

WINPIXELDLL WPX_Sprite *WINPIXELCALL wpx_sprite_load (const char *path) {

    int w, h, channels;

    FILE *f = fopen(path, "rb");
    if (!f) {
        printf("wpx_sprite_load: cannot open \"%s\"\n", path);
        return NULL;
    }
    fclose(f);

    unsigned char *raw = stbi_load(path, &w, &h, &channels, 0);
    if (!raw) {
        printf("wpx_sprite_load: stbi_load failed \"%s\"\n", path);
        return NULL;
    }

    WPX_Sprite *s  = calloc(1, sizeof(WPX_Sprite));
    if (!s) { stbi_image_free(raw); return NULL; }
    s->w       = w;
    s->h       = h;
    s->pixels  = calloc(w * h, sizeof(uint32_t));
    if (!s->pixels) { free(s); stbi_image_free(raw); return NULL; }

    for (int i = 0, j = 0; i < w * h; i++, j += channels) {
        uint8_t r = raw[j + 0];
        uint8_t g = raw[j + 1];
        uint8_t b = raw[j + 2];
        uint8_t a = (channels >= 4) ? raw[j + 3] : 0xFF;
        s->pixels[i] = WRGBA(r, g, b, a);
    }

    stbi_image_free(raw);

    return s;
}

WINPIXELDLL void WINPIXELCALL wpx_sprite_free (WPX_Sprite *s) {
    if (!s) return;
    free(s->pixels);
    free(s);
}

/* ---------------------------------------------------------------
 * Pixel access
 * --------------------------------------------------------------- */

WINPIXELDLL void WINPIXELCALL wpx_sprite_set_pixel (WPX_Sprite *s, int x, int y, Color32 color) {
    if (!s || x < 0 || x >= s->w || y < 0 || y >= s->h) return;
    s->pixels[y * s->w + x] = color;
}

WINPIXELDLL Color32 WINPIXELCALL wpx_sprite_get_pixel (const WPX_Sprite *s, int x, int y) {
    if (!s || x < 0 || x >= s->w || y < 0 || y >= s->h) return BLANK;
    return s->pixels[y * s->w + x];
}

WINPIXELDLL void WINPIXELCALL wpx_sprite_fill (WPX_Sprite *s, Color32 color) {
    if (!s) return;
    for (int i = 0; i < s->w * s->h; i++) s->pixels[i] = color;
}

WINPIXELDLL void WINPIXELCALL wpx_sprite_clear (WPX_Sprite *s) {
    if (!s) return;
    memset(s->pixels, 0, s->w * s->h * sizeof(uint32_t));
}

/* ---------------------------------------------------------------
 * Draw
 * --------------------------------------------------------------- */

WINPIXELDLL void WINPIXELCALL wpx_sprite_draw (const WPX_Sprite *s, int x, int y) {
    if (!s) return;
    for (int sy = 0; sy < s->h; sy++)
        for (int sx = 0; sx < s->w; sx++) {
            Color32 c = s->pixels[sy * s->w + sx];
            if (_WPX_IS_COLORKEY(c) || (c & 0xFF) == 0) continue;
            wpx_pixel(x + sx, y + sy, c);
        }
}

WINPIXELDLL void WINPIXELCALL wpx_sprite_draw_scale (const WPX_Sprite *s, int x, int y, int scale) {
    if (!s || scale <= 0) return;
    for (int sy = 0; sy < s->h; sy++)
        for (int sx = 0; sx < s->w; sx++) {
            Color32 c = s->pixels[sy * s->w + sx];
            if (_WPX_IS_COLORKEY(c) || (c & 0xFF) == 0) continue;
            for (int dy = 0; dy < scale; dy++)
                for (int dx = 0; dx < scale; dx++)
                    wpx_pixel(x + sx * scale + dx, y + sy * scale + dy, c);
        }
}

WINPIXELDLL void WINPIXELCALL wpx_sprite_draw_flip (const WPX_Sprite *s, int x, int y, bool flip_x, bool flip_y) {
    if (!s) return;
    for (int sy = 0; sy < s->h; sy++)
        for (int sx = 0; sx < s->w; sx++) {
            Color32 c = s->pixels[sy * s->w + sx];
            if (_WPX_IS_COLORKEY(c) || (c & 0xFF) == 0) continue;
            int dx = flip_x ? (s->w - 1 - sx) : sx;
            int dy = flip_y ? (s->h - 1 - sy) : sy;
            wpx_pixel(x + dx, y + dy, c);
        }
}

/* draw a sub-region (spritesheet / atlas) */
WINPIXELDLL void WINPIXELCALL wpx_sprite_draw_sub (const WPX_Sprite *s, int x, int y, recti src) {
    if (!s) return;
    for (int sy = 0; sy < src.h; sy++)
        for (int sx = 0; sx < src.w; sx++) {
            int px = src.x + sx;
            int py = src.y + sy;
            if (px < 0 || px >= s->w || py < 0 || py >= s->h) continue;
            Color32 c = s->pixels[py * s->w + px];
            if (_WPX_IS_COLORKEY(c) || (c & 0xFF) == 0) continue;
            wpx_pixel(x + sx, y + sy, c);
        }
}

#undef _WPX_IS_COLORKEY
