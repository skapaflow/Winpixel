#include <math.h>
#include <stdio.h>
#include <winpixel.h>

vec2f vec2f_add (vec2f v1, vec2f v2) {

	return vec2f(v1.x + v2.x, v1.y + v2.y);
}

vec2f vec2f_scale (vec2f v, float scale) {

	return (vec2f) {v.x*scale, v.y*scale};
}

vec2f vec2f_rot (vec2f v, float angle) {

	vec2f result = {0};

	float cosres = cosf(angle);
	float sinres = sinf(angle);

	result.x = v.x*cosres - v.y*sinres;
	result.y = v.x*sinres + v.y*cosres;

	return result;
}

void wpx_equilateral_triangle_gen (vec2f *vf, float scale) {

    // Height of an equilateral triangle: scale * (sqrt(3) / 2)
    float height = scale * (sqrt(3.0f) / 2.0f);
    // Top Vertex (0, -2/3 of height to keep it centered)
    vf[0].x = 0.0f;
    vf[0].y = -(height * 2.0f / 3.0f);
    // Bottom Right Vertex
    vf[1].x = scale / 2.0f;
    vf[1].y = height / 3.0f;
    // Bottom Left Vertex
    vf[2].x = -scale / 2.0f;
    vf[2].y = height / 3.0f;
}

int main (void) {

	winpixel_window ("Triangle", 800, 600, true);

	Coloriof color;

	float fscale = 1.0f;
	vec2f triangle[3] = {0};

	wpx_equilateral_triangle_gen(triangle, 100);

	while (wpx_quit && !wpx_key_down(VK_ESCAPE)) {

		const float speed = 0.7f;
		static float tmp = 1.0f;
		if (wpx_mouse_wheel < 0) tmp *= speed;     /* zoom out */
		if (wpx_mouse_wheel > 0) tmp  = tmp/speed; /* zoom in */
		fscale = lerp(fscale, tmp, 0.234f);

		static float rot = 0.0f;
		rot += 0.005f;

		vec2f v[3] = {0};
		vec2i vert[3] = {0};

		for (int i = 0; i < 3; i++) {
			v[i].x = triangle[i].x;
			v[i].y = triangle[i].y;
			v[i] = vec2f_scale(v[i], fscale);
			v[i] = vec2f_rot(v[i], rot);
			v[i] = vec2f_add(v[i], vec2f(wpx_mouse.x, wpx_mouse.y));
			vert[i].x = v[i].x;
			vert[i].y = v[i].y;
		}

		if (!random(100))
			color.in = hex_to_Colorf(hrandom());
		color.out = lerp_color(color.out, color.in, 0.01f);

		wpx_circle_fill(WPX_W2, WPX_H2, 100, WHITE);
		wpx_triangle_fill_grid(vert, Colorf_to_hex(color.out));

		Color32 newcolor = Colorf_to_hex(color.out);
		wpx_text(newcolor, 1, 10, 10, "SCALE %.2f NEXT_COLOR(%08X)", fscale, newcolor);

		winpixel_render(BLUEPRINT, 1);
	}

	return 0;
}