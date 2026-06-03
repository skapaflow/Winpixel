#include "winpixel_internal.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#define WPX_ASCII_LEN 96
#define WPX_ASCII_SIZE 37

#define WPX_MAX_TEXT 512

/* "char pixel size" = 6x6 */
const char wpx_font_char[WPX_ASCII_LEN][WPX_ASCII_SIZE] = {
	/* NULL PART */
	/* {,} */
	/* !"#$%&'()*+,-./ */
	{"000000000000000000000000000000000000"},/*   */ /* 32 */
	{"001100001100001100001100000000001100"},/* ! */
	{"010100010100000000000000000000000000"},/* " */
	{"010100111110010100010100111110010100"},/* # */
	{"001000011110101000011100001010111100"},/* $ */
	{"011001101010110100001011010101100110"},/* % */
	{"011000100100011001100101100010011101"},/* & */
	{"001000001000000000000000000000000000"},/* ' */
	{"000100001000001000001000001000000100"},/* ( */
	{"001000000100000100000100000100001000"},/* ) */
	{"101010011100111110011100101010000000"},/* * */
	{"000000001000001000111110001000001000"},/* + */
	{"000000000000000000001100001100011000"},/* , */
	{"000000000000000000111110000000000000"},/* - */
	{"000000000000000000000000001100001100"},/* . */
	{"000000000010000100001000010000100000"},/* / */ /* 47 */
	/* 0123456789 */
	{"011100100110101010110010100010011100"},/* 0 */ /* 48 */
	{"001000011000101000001000001000111110"},/* 1 */
	{"011100100010000100001000010000111110"},/* 2 */
	{"011100100010001100000010100010011100"},/* 3 */
	{"000100001100010100111110000100000100"},/* 4 */
	{"111110100000111100000010100010011100"},/* 5 */
	{"011100100000111100100010100010011100"},/* 6 */
	{"111110000010000100001000010000100000"},/* 7 */
	{"011100100010011100100010100010011100"},/* 8 */
	{"011100100010100010011110000010011100"},/* 9 */ /* 57 */
	/* :;<=>?@ */
 // {"001100001100000000001100001100000000"},/* : */ /* 58 */
	{"001100001100000000000000001100001100"},/* : */ /* 58 */
	{"001100001100000000001100001100011000"},/* ; */
	{"000000000100001000010000001000000100"},/* < */
	{"000000000000011110000000011110000000"},/* = */
	{"000000001000000100000010000100001000"},/* > */
	{"011100100010001100001000000000001000"},/* ? */
	{"011100100010101110101010101100011110"},/* @ */ /* 64 */
	/* ABCDEFGHIJKLMNOPQRSTUVWXYZ */
	{"001000001000010100010100111110100010"},/* A */ /* 65 */
	{"111100100010111100100010100010111100"},/* B */
	{"011100100010100000100000100010011100"},/* C */
	{"111100100010100010100010100010111100"},/* D */
	{"111110100000111100100000100000111110"},/* E */
	{"111110100000111100100000100000100000"},/* F */
	{"011100100010100000101110100010011100"},/* G */
	{"100010100010111110100010100010100010"},/* H */
	{"011100001000001000001000001000011100"},/* I */
	{"111110000010000010100010100010011100"},/* J */
	{"100100101000110000101000100100100010"},/* K */
	{"100000100000100000100000100000111110"},/* L */
	{"100010110110101010100010100010100010"},/* M */
	{"100010110010101010100110100010100010"},/* N */
	{"011100100010100010100010100010011100"},/* O */
	{"111100100010100010111100100000100000"},/* P */
	{"011100100010100010100010100110011111"},/* Q */
	{"111100100010100010111100100010100010"},/* R */
	{"011100100010010000001100100010011100"},/* S */
	{"111110001000001000001000001000001000"},/* T */
	{"100010100010100010100010100010011100"},/* U */
	{"100010100010010100010100001000001000"},/* V */
	{"100010101010101010110110110110100010"},/* W */
	{"100010010100001000001000010100100010"},/* X */
	{"100010010100001000001000001000001000"},/* Y */
	{"111110000100001000010000100000111110"},/* Z */ /* 90 */
	/* [\]^_` */
	{"001110001000001000001000001000001110"},/* [ */ /* 91 */
	{"000000100000010000001000000100000010"},/* \ */
	{"011100000100000100000100000100011100"},/* ] */
	{"001000010100100010000000000000000000"},/* ^ */
	{"000000000000000000000000000000111110"},/* _ */
	{"110000011000000000000000000000000000"},/* ` */ /* 96 */
	/* abcdefghijklmnopqrstuvwxyz */
	{"000000011100000010011110100110011010"},/* a */ /* 97 */
	{"100000100000111100100010110010101100"},/* b */
	{"000000000000011100100000100000011100"},/* c */
	{"000010000010011110100010100110011010"},/* d */
	{"000000011100100010111110100000011100"},/* e */
	{"001110010000111100010000010000010000"},/* f */
	{"011010100110100010011010000010011100"},/* g */
	{"000000010000010000011100010010010010"},/* h */
	{"001000000000011000001000001000011100"},/* i */
	{"000100000000011100000100000100011000"},/* j */
	{"000000010000010010010100011000010110"},/* k */
	{"000000011000001000001000001000011100"},/* l */
	{"000000010100101010101010101010101010"},/* m */
	{"000000101100110010100010100010100010"},/* n */
	{"000000011100100010100010100010011100"},/* o */
	{"000000101100110010100010111100100000"},/* p */
	{"000000011010100110100010011110000010"},/* q */
	{"000000101100110010100000100000100000"},/* r */
	{"000000001110010000001100000010011100"},/* s */
	{"010000111100010000010000010000001100"},/* t */
	{"000000100010100010100010100110011010"},/* u */
	{"000000000000100010100010010100001000"},/* v */
	{"000000000000100010101010101010010100"},/* w */
	{"000000000000110110001000001000110110"},/* x */
	{"000000000000100010010100001000110000"},/* y */
	{"000000000000111100001000010000111100"},/* z */ /* 122 */
	/* {|}~█ */
	{"000110001000001000110000001000001110"},/* { */ /* 123 */
	{"001000001000001000001000001000001000"},/* | */
	{"011000000100000100000011000100011100"},/* } */
	{"000000000000011010101100000000000000"},/* ~ */
	{"011110011110011110011110011110011110"} /* █ */ /* 127 */
};

/* Writes a size×size block directly to the framebuffer; clips to screen bounds. */
static inline void _text_block(int x, int y, int size, Color32 px) {
	int x1 = x + size - 1, y1 = y + size - 1;
	if (x1 < 0 || x >= wpx_render.w || y1 < 0 || y >= wpx_render.h) return;
	if (x  < 0) x = 0;
	if (y  < 0) y = 0;
	if (x1 >= wpx_render.w) x1 = wpx_render.w - 1;
	if (y1 >= wpx_render.h) y1 = wpx_render.h - 1;
	int w = x1 - x + 1;
	for (int row = y; row <= y1; row++) {
		Color32 *p = wpx_render.buffer_screen + row * wpx_render.w + x;
		for (int i = 0; i < w; i++) *p++ = px;
	}
}

void WINPIXELCALL wpx_text_standard (
	int         center,
	Color32     color,
	int         scale,
	Color32     bcolor,
	int         gap,
	int         x,
	int         y,
	const char *string) {

	const int begin = 32;
	scale += 1;
	const int wsize = scale * 6;

	int str_len = (int)strlen(string);
	int w_pix   = center ? scale * str_len * 6 / 2 : 0;
	int h_pix   = center ? scale * 6 / 2           : 0;

	Color32 fg_px = wpx_color_to_pixel(color);
	Color32 bg_px = wpx_color_to_pixel(bcolor);

	for (int i = 0; i < str_len; i++) {
		int ci = (int)(unsigned char)string[i] - begin;
		if (ci < 0 || ci >= WPX_ASCII_LEN) ci = WPX_ASCII_LEN - 1;
		const char *glyph = wpx_font_char[ci];

		int base_x = x + wsize * i - w_pix;
		int base_y = y - h_pix;

		for (int gy = 0; gy < 6; gy++) {
			int py = base_y + gy * scale;
			for (int gx = 0; gx < 6; gx++) {
				if (glyph[gx + gy * 6] == '1') {
					int px = base_x + gx * scale;
					if (gap) _text_block(px + gap, py + gap, scale, bg_px);
					_text_block(px, py, scale, fg_px);
				}
			}
		}
	}
}

WINPIXELDLL void WINPIXELCALL wpx_text_ex (
	int         bool_center,
	Color32     foreground_color,
	int         scale,
	Color32     background_color,
	int         gap,
	int         x,
	int         y,
	const char *fmt, ...) {

	va_list va;
	char buf[WPX_MAX_TEXT] = {0};
	va_start(va, fmt);
		vsnprintf(buf, WPX_MAX_TEXT, fmt, va);
		wpx_text_standard(bool_center, foreground_color, scale, background_color, gap, x, y, buf);
	va_end(va);
}

WINPIXELDLL void WINPIXELCALL wpx_text_center (
	Color32     color,
	int         scale,
	int         x,
	int         y,
	const char *fmt, ...) {

	va_list va;
	char buf[WPX_MAX_TEXT] = {0};
	va_start(va, fmt);
		vsnprintf(buf, WPX_MAX_TEXT, fmt, va);
		wpx_text_standard(1, color, scale, 0x000000, 1, x, y, buf);
	va_end(va);
}

WINPIXELDLL void WINPIXELCALL wpx_text (Color32 color, int scale, int x, int y, const char *fmt, ...) {

	va_list va;
	char buf[WPX_MAX_TEXT] = {0};
	va_start(va, fmt);
		vsnprintf(buf, WPX_MAX_TEXT, fmt, va);
		wpx_text_standard(0, color, scale, 0x000000, 1, x, y, buf);
	va_end(va);
}

WINPIXELDLL void WINPIXELCALL wpx_text_measure (const char *str, int scale, int *w, int *h) {

	const int len = 6;
	scale += 1;
	if (w) *w = scale * len * (int)strlen(str);
	if (h) *h = scale * len;
}

// printf-style formatter backed by a ring of static buffers.
// Returned pointer stays valid for CAT_POOL_SIZE more calls.
const char *cat (const char *fmt, ...) {

#ifndef CAT_POOL_SIZE
    #define CAT_POOL_SIZE   10      // number of rotating slots
#endif
#ifndef CAT_BUF_SIZE
    #define CAT_BUF_SIZE  1024      // bytes per slot
#endif

    static char pool[CAT_POOL_SIZE][CAT_BUF_SIZE];
    static int  slot = 0;

    char *buf = pool[slot];
    memset(buf, 0, CAT_BUF_SIZE);

    if (fmt != NULL) {
        va_list args;
        va_start(args, fmt);
        int written = vsnprintf(buf, CAT_BUF_SIZE, fmt, args);
        va_end(args);

        if (written >= CAT_BUF_SIZE) {
            // overflow: replace last 3 chars with "..."
            char *tail = buf + CAT_BUF_SIZE - 4;
            snprintf(tail, 4, "...");
        }

        if (++slot >= CAT_POOL_SIZE) slot = 0;
    }

    return buf;
}