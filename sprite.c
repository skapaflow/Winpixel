#include <winpixel.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

/*
StretchDIBits (
	HDC hdc,
	int xDest,
	int yDest,
	int DestWidth,
	int DestHeight,
	int xSrc,
	int ySrc,
	int SrcWidth,
	int SrcHeight,
	CONST VOID *lpBits,
	CONST BITMAPINFO *lpbmi,
	UINT iUsage,
	DWORD rop
);
*/
void WINPIXELCALL wpx_draw_image (uint32_t *data, rec4i src, rec4i des) {

	static BITMAPINFO d;
	d.bmiHeader.biSize = sizeof(d.bmiHeader);
	d.bmiHeader.biWidth = src.w;
	d.bmiHeader.biHeight = src.h;
	d.bmiHeader.biPlanes = 1;
	d.bmiHeader.biBitCount = 32;
	d.bmiHeader.biCompression = BI_RGB;

	SetStretchBltMode(wpx_render.screen_dc, HALFTONE);
	// SetStretchBltMode(wpx_render.screen_dc, BLACKONWHITE);
	StretchDIBits(wpx_render.screen_dc, des.x, des.y, des.w, des.h, src.x, src.y, src.w, src.h, data, &d, DIB_RGB_COLORS, SRCCOPY);
}

#define SET(w,x,y) (int) (abs(x) + (abs(y) * w))

void flip_vertical (uint32_t *array, int w, int h) {

	if (w <= 0 || h <= 0)
		return;

	if (array) {
		for (int y = 0; y < h/2; y++)
			for (int x = 0; x < w; x++)
				WPXSWAP(uint32_t, array[SET(w,x,y)], array[SET(w,x,(h-y-1))]);
	} else
		printf("flip_vertical - *array = NULL");
}

uint32_t WINPIXELCALL *wpx_load_image (const char *name, int *w, int *h) {

	int channels;

	FILE *f = fopen(name, "rb");
	if (!f) {
		printf("cannot open \"%s\"", name);
		return NULL;
	}
	wpx_fclose(f);

	if (!stbi_info(name, w, h, &channels)) {
		printf("cannot open \"%s\"", name);
		return NULL;
	}

	/* load image */
	unsigned char *raw = stbi_load(name, w, h, &channels, 0);

	if (raw == NULL) {
		printf("cannot open \"%s\", raw == NULL", name);
		return NULL;
	}

	/* transfer image */
	uint32_t *rgb = NULL;
	wpx_calloc(rgb, (*w * *h));

	for (int i = 0, j = 0; i < (*w * *h); i++, j += channels)
		rgb[i] = WRGB((int)raw[j+0], (int)raw[j+1], (int)raw[j+2]);

	flip_vertical(rgb, *w, *h);
	stbi_image_free(raw);

	return rgb;
}