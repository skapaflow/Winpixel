/*
#getline {
	clang -o trails trails.c -s -Wall
	-IC:/WINPIXEL
	-LC:/WINPIXEL/lib
	-lwinpixel
	|@trails.exe
}
*/

#include <stdbool.h>
#include <winpixel.h>

typedef struct {
	int lot;
	int step;
	vec2f v[256];
} VEC_TAIL;

uint32_t gradient_index (uint32_t color1, uint32_t color2, int index, int lot) {

	wpx_RGB start = hex_to_rgb(color1);
	wpx_RGB end   = hex_to_rgb(color2);
	float t = (float)index / lot;
	float r = start.r + t * (end.r - start.r);
	float g = start.g + t * (end.g - start.g);
	float b = start.b + t * (end.b - start.b);
	return rgb_to_hex((int)r, (int)g, (int)b);
}

void vector_tail (VEC_TAIL *vt, vec2f pos, int len, float time, uint32_t color) {

	#define DEBUG 0

	/* limite do vetor em VEC_TAIL */
	len = __min(len, (sizeof(vt->v) / sizeof((vt->v)[0])));

#if 0
	if (vt->lot <= len && vt->v[0].x > 0 && vt->v[0].y > 0) {
		vec2f v1 = {     pos.x,      pos.y};
		vec2f v2 = {vt->v[0].x, vt->v[0].y};
		wpx_circle_fill(v1.x, v1.y, 4, color);
		wpx_spline(v1, v2, 10, color);
		// wpx_line(pos.x, pos.y, vt->v[0].x, vt->v[0].y, color);
	}
#endif

	/* draw */
	for (int i = 0; i < (vt->lot - 1); i++) {

		/* DEBUG */
		#if DEBUG
			if (vt->step > time)
				printf("%d,%d|", (int)vt->v[i].x, (int)vt->v[i].y);
		#endif

		uint32_t c = gradient_index(color, BLACK, i, vt->lot);

		if (vt->v[i   % vt->lot].x > 0 &&
			vt->v[i   % vt->lot].y > 0 &&
			vt->v[i+1 % vt->lot].x > 0 &&
			vt->v[i+1 % vt->lot].y > 0) {
			vec2f v1 = {((int)vt->v[i   % vt->lot].x + 1), ((int)vt->v[i   % vt->lot].y + 1)};
			vec2f v2 = {((int)vt->v[i+1 % vt->lot].x + 1), ((int)vt->v[i+1 % vt->lot].y + 1)};
			wpx_circle_fill(v1.x, v1.y, 4, c);
			wpx_spline(v1.x, v1.y, v2.x, v2.y, 10, c);
			// wpx_line(v1.x, v1.y, v2.x, v2.y, c);
		}
	}

	/* add */
	if (vt->step++ > time) {
		#if DEBUG
			printf("\n");
		#endif
		vt->step = 0;
		/* expandir lista */
		vt->lot += (vt->lot < len ? 1 : -1);
		vt->lot = __max(vt->lot, 0);
		/* Move os elementos para a direita */
		for (int j = (vt->lot - 1); j > 0; j--)
			vt->v[j] = vt->v[j - 1];
		/* adicionar no inicio */
		if (vt->lot <= len) {
			vt->v[0].x = pos.x;
			vt->v[0].y = pos.y;
		} else {
			vt->v[0].x = 0;
			vt->v[0].y = 0;
		}
	}

	#undef DEBUG
}

int main (void) {

	winpixel_window("Trails", 800, 700);

	VEC_TAIL vt = {0};

	WPX_NOISEGEN ns[3];

	noise_generator(&ns[0], 0.001f);
	noise_generator(&ns[1], 0.01f);
	noise_generator(&ns[2], 0.01f);

	while (wpx_exit && !key(VK_ESCAPE)) {

		float r2 = noise(&ns[0]);

		int r = map(r2, -1, 1, 0, 255);
		int g = map(noise(&ns[1]), -1, 1, 0, 255);
		int b = map(noise(&ns[2]), -1, 1, 0, 255);
		uint32_t color = rgb_to_hex(r, g, b);
		// printf("[%d/%d/%d] %.2f\n", r, g, b, r2);

		vector_tail(&vt, (vec2f){wpx_mouse.x, wpx_mouse.y}, 50, 0, color);

		winpixel_render(BLACK, 2);
	}

	winpixel_close();

	return 0;
}