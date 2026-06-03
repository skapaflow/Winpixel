#include "winpixel_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

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

    unsigned char *raw = stbi_load(path, &w, &h, &channels, 0);
    if (!raw) {
        printf("wpx_sprite_load: cannot load \"%s\"\n", path);
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
    int scr_w = wpx_render.w, scr_h = wpx_render.h;
    int sx0 = (x < 0) ? -x : 0;
    int sy0 = (y < 0) ? -y : 0;
    int sx1 = (x + s->w > scr_w) ? scr_w - x : s->w;
    int sy1 = (y + s->h > scr_h) ? scr_h - y : s->h;
    if (sx0 >= sx1 || sy0 >= sy1) return;

    for (int sy = sy0; sy < sy1; sy++) {
        const Color32 *src = s->pixels + sy * s->w + sx0;
        Color32 *dst = wpx_render.buffer_screen + (y + sy) * scr_w + (x + sx0);
        for (int sx = sx0; sx < sx1; sx++, src++, dst++) {
            Color32 c = *src;
            if (_WPX_IS_COLORKEY(c) || (c & 0xFF) == 0) continue;
            *dst = wpx_color_to_pixel(c);
        }
    }
}

WINPIXELDLL void WINPIXELCALL wpx_sprite_draw_scale (const WPX_Sprite *s, int x, int y, int scale) {
    if (!s || scale <= 0) return;
    int scr_w = wpx_render.w, scr_h = wpx_render.h;
    for (int sy = 0; sy < s->h; sy++) {
        int by0 = y + sy * scale, by1 = by0 + scale;
        if (by1 <= 0) continue;
        if (by0 >= scr_h) break;
        if (by0 < 0) by0 = 0;
        if (by1 > scr_h) by1 = scr_h;
        for (int sx = 0; sx < s->w; sx++) {
            Color32 c = s->pixels[sy * s->w + sx];
            if (_WPX_IS_COLORKEY(c) || (c & 0xFF) == 0) continue;
            int bx0 = x + sx * scale, bx1 = bx0 + scale;
            if (bx1 <= 0) continue;
            if (bx0 >= scr_w) break;
            if (bx0 < 0) bx0 = 0;
            if (bx1 > scr_w) bx1 = scr_w;
            Color32 px = wpx_color_to_pixel(c);
            for (int by = by0; by < by1; by++) {
                Color32 *row = wpx_render.buffer_screen + by * scr_w + bx0;
                for (int bx = bx0; bx < bx1; bx++) *row++ = px;
            }
        }
    }
}

WINPIXELDLL void WINPIXELCALL wpx_sprite_draw_flip (const WPX_Sprite *s, int x, int y, bool flip_x, bool flip_y) {
    if (!s) return;
    int scr_w = wpx_render.w, scr_h = wpx_render.h;
    for (int sy = 0; sy < s->h; sy++) {
        int dy = flip_y ? (s->h - 1 - sy) : sy;
        int dst_y = y + dy;
        if ((unsigned)dst_y >= (unsigned)scr_h) continue;
        for (int sx = 0; sx < s->w; sx++) {
            Color32 c = s->pixels[sy * s->w + sx];
            if (_WPX_IS_COLORKEY(c) || (c & 0xFF) == 0) continue;
            int dx = flip_x ? (s->w - 1 - sx) : sx;
            int dst_x = x + dx;
            if ((unsigned)dst_x >= (unsigned)scr_w) continue;
            wpx_render.buffer_screen[dst_y * scr_w + dst_x] = wpx_color_to_pixel(c);
        }
    }
}

/* draw a sub-region (spritesheet / atlas) */
WINPIXELDLL void WINPIXELCALL wpx_sprite_draw_sub (const WPX_Sprite *s, int x, int y, recti src) {
    if (!s) return;
    /* clip src rect to sprite bounds */
    int sx0 = src.x < 0 ? 0 : src.x;
    int sy0 = src.y < 0 ? 0 : src.y;
    int sx1 = src.x + src.w > s->w ? s->w : src.x + src.w;
    int sy1 = src.y + src.h > s->h ? s->h : src.y + src.h;
    if (sx0 >= sx1 || sy0 >= sy1) return;

    int scr_w = wpx_render.w, scr_h = wpx_render.h;
    for (int py = sy0; py < sy1; py++) {
        int dst_y = y + (py - src.y);
        if ((unsigned)dst_y >= (unsigned)scr_h) continue;
        for (int px = sx0; px < sx1; px++) {
            Color32 c = s->pixels[py * s->w + px];
            if (_WPX_IS_COLORKEY(c) || (c & 0xFF) == 0) continue;
            int dst_x = x + (px - src.x);
            if ((unsigned)dst_x >= (unsigned)scr_w) continue;
            wpx_render.buffer_screen[dst_y * scr_w + dst_x] = wpx_color_to_pixel(c);
        }
    }
}

#undef _WPX_IS_COLORKEY

/* ---------------------------------------------------------------
 * Screenshot
 * --------------------------------------------------------------- */

WINPIXELDLL void WINPIXELCALL wpx_write_png (const WPX_Sprite *sprite, const char *name) {
    if (!sprite) return;
    int w = sprite->w;
    int h = sprite->h;
    unsigned char *data = malloc((size_t)w * h * 4);
    if (!data) return;
    for (int i = 0; i < w * h; i++) {
        uint32_t c      = sprite->pixels[i];
        data[i * 4 + 0] = (c >> 24) & 0xFF;
        data[i * 4 + 1] = (c >> 16) & 0xFF;
        data[i * 4 + 2] = (c >>  8) & 0xFF;
        data[i * 4 + 3] = (c      ) & 0xFF;
    }
    stbi_write_png(name, w, h, 4, data, w * 4);
    free(data);
}

WINPIXELDLL void WINPIXELCALL wpx_screenshot (const char *name) {
    int w = wpx_render.w, h = wpx_render.h;
    /* buffer_screen is 0x00RRGGBB (GDI format) — write 3-channel RGB PNG */
    unsigned char *data = malloc((size_t)w * h * 3);
    if (!data) return;
    for (int i = 0; i < w * h; i++) {
        uint32_t c      = wpx_render.buffer_screen[i];
        data[i * 3 + 0] = (c >> 16) & 0xFF;
        data[i * 3 + 1] = (c >>  8) & 0xFF;
        data[i * 3 + 2] = (c      ) & 0xFF;
    }
    stbi_write_png(name, w, h, 3, data, w * 3);
    free(data);
}