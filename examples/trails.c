#include <math.h>
#include <winpixel.h>

typedef struct {
	int lot;
	int step;
	vec2f v[256];
} VEC_TAIL;

uint32_t gradient_index (uint32_t color1, uint32_t color2, int index, int lot) {

	Colorf start = hex_to_Colorf(color1);
	Colorf end   = hex_to_Colorf(color2);
	float t = (float)index / lot;
	float r = start.r + t * (end.r - start.r);
	float g = start.g + t * (end.g - start.g);
	float b = start.b + t * (end.b - start.b);
	float a = start.a + t * (end.a - start.a);
	return rgba_to_hex((int)r, (int)g, (int)b, (int)a);
}

void vector_tail (VEC_TAIL *vt, vec2f pos, int len, float time, uint32_t color) {

	#define DEBUG 0

	/* clamp to VEC_TAIL's array size */
	len = fminf(len, (sizeof(vt->v) / sizeof((vt->v)[0])));

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
		/* grow the list */
		vt->lot += (vt->lot < len ? 1 : -1);
		vt->lot = fmaxf(vt->lot, 0);
		/* shift elements to the right */
		for (int j = (vt->lot - 1); j > 0; j--)
			vt->v[j] = vt->v[j - 1];
		/* add at the front */
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

	winpixel_window("Trails", 800, 700, false);

	VEC_TAIL vt = {0};

	Coloriof color = {hex_to_Colorf(hrandom()), hex_to_Colorf(hrandom())};

	while (wpx_quit && !wpx_key_down(VK_ESCAPE)) {

		if (!random(100))
			color.in = hex_to_Colorf(hrandom());
		color.out = lerp_color(color.out, color.in, 0.1f);

		vector_tail(&vt, (vec2f){wpx_mouse.x, wpx_mouse.y}, 50, 0, Colorf_to_hex(color.out));

		wpx_text(WHITE, 1, 10, 10, "COLOR ");
		wpx_text(Colorf_to_hex(color.out), 1, 85, 10, "%08X", Colorf_to_hex(color.out));

		winpixel_render(BLACK, 2);
	}

	return 0;
}