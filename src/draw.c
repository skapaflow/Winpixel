#include "winpixel_internal.h"

#include <math.h>


WINPIXELDLL Color32 WINPIXELCALL rgba_to_hex (int r, int g, int b, int a) {

	return ((r & 0xff) << 24) + ((g & 0xff) << 16) + ((b & 0xff) << 8) + (a & 0xff);
}

WINPIXELDLL Colorf WINPIXELCALL hex_to_Colorf (Color32 h) {

	return (Colorf) {
		.r =  h               >> 24,
		.g = (h & 0x00ff0000) >> 16,
		.b = (h & 0x0000ff00) >>  8,
		.a = (h & 0x000000ff)
	};
}

WINPIXELDLL Color32 WINPIXELCALL Colorf_to_hex (Colorf color) {

	return (((int)color.r & 0xff) << 24) +
           (((int)color.g & 0xff) << 16) +
           (((int)color.b & 0xff) << 8) +
            ((int)color.a & 0xff);
}

WINPIXELDLL Colorf WINPIXELCALL lerp_color (Colorf start, Colorf end, float time) {
    
	start.r = lerp(start.r, end.r, time);
	start.g = lerp(start.g, end.g, time);
	start.b = lerp(start.b, end.b, time);
	start.a = lerp(start.a, end.a, time);
	return start;
}

WINPIXELDLL void WINPIXELCALL winpixel_clear (Color32 color) {
    Color32 px = wpx_color_to_pixel(color);
    int total   = wpx_render.w * wpx_render.h;
    for (int i = 0; i < total; i++) wpx_render.buffer_screen[i] = px;
}

WINPIXELDLL void WINPIXELCALL wpx_pixel (int x, int y, Color32 color) {
    if (x < 0 || x >= wpx_render.w || y < 0 || y >= wpx_render.h) return;
    wpx_render.buffer_screen[y * wpx_render.w + x] = wpx_color_to_pixel(color);
}

WINPIXELDLL Color32 WINPIXELCALL wpx_getpixel (int x, int y) {
    if (x < 0 || x >= wpx_render.w || y < 0 || y >= wpx_render.h) return BLANK;
    Color32 px = wpx_render.buffer_screen[y * wpx_render.w + x];
    uint8_t r = (px >> 16) & 0xFF;
    uint8_t g = (px >>  8) & 0xFF;
    uint8_t b =  px        & 0xFF;
    return ((Color32)r << 24) | ((Color32)g << 16) | ((Color32)b << 8) | 0xFF;
}

/* Fills a horizontal span directly into the framebuffer; clips and skips out-of-bounds rows. */
static inline void _span_fill(int x0, int x1, int y, Color32 px) {
    if (y < 0 || y >= wpx_render.h) return;
    if (x0 > x1) { int t = x0; x0 = x1; x1 = t; }
    if (x0 >= wpx_render.w || x1 < 0) return;
    if (x0 < 0) x0 = 0;
    if (x1 >= wpx_render.w) x1 = wpx_render.w - 1;
    Color32 *row = wpx_render.buffer_screen + y * wpx_render.w;
    for (int x = x0; x <= x1; x++) row[x] = px;
}

/* Same but screen-door: only pixels where (x+y) is odd (checkerboard 50% transparency). */
static inline void _span_fill_grid(int x0, int x1, int y, Color32 px) {
    if (y < 0 || y >= wpx_render.h) return;
    if (x0 > x1) { int t = x0; x0 = x1; x1 = t; }
    if (x0 >= wpx_render.w || x1 < 0) return;
    if (x0 < 0) x0 = 0;
    if (x1 >= wpx_render.w) x1 = wpx_render.w - 1;
    if (!((x0 + y) & 1)) x0++;
    Color32 *row = wpx_render.buffer_screen + y * wpx_render.w;
    for (int x = x0; x <= x1; x += 2) row[x] = px;
}

WINPIXELDLL void WINPIXELCALL wpx_line (int x0, int y0, int x1, int y1, Color32 color) {

    /* Cohen-Sutherland: clip line to screen bounds, then Bresenham without per-pixel bounds check. */
    #define _CS(x, y) \
        (((x) < 0) | (((x) >= wpx_render.w) << 1) | (((y) < 0) << 2) | (((y) >= wpx_render.h) << 3))

    int c0 = _CS(x0, y0), c1 = _CS(x1, y1);
    while (c0 | c1) {
        if (c0 & c1) return;
        int c = c0 ? c0 : c1, cx, cy;
        int dx = x1 - x0, dy = y1 - y0;
        if      (c & 8) { cy = wpx_render.h - 1; cx = x0 + dx * (cy - y0) / dy; }
        else if (c & 4) { cy = 0;                cx = x0 + dx * (cy - y0) / dy; }
        else if (c & 2) { cx = wpx_render.w - 1; cy = y0 + dy * (cx - x0) / dx; }
        else            { cx = 0;                cy = y0 + dy * (cx - x0) / dx; }
        if (c == c0) { x0 = cx; y0 = cy; c0 = _CS(x0, y0); }
        else         { x1 = cx; y1 = cy; c1 = _CS(x1, y1); }
    }
    #undef _CS

    Color32 px  = wpx_color_to_pixel(color);
    int dx      = abs(x1 - x0);
    int dy      = abs(y1 - y0);
    int sx      = (x0 < x1) ? 1 : -1;
    int sy      = (y0 < y1) ? wpx_render.w : -wpx_render.w;
    int err     = dx - dy;
    Color32 *buf = wpx_render.buffer_screen + y0 * wpx_render.w + x0;

    while (1) {
        *buf = px;
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x0 += sx; buf += sx; }
        if (e2 <  dx) { err += dx; y0 += (sy > 0 ? 1 : -1); buf += sy; }
    }
}

/*
 * A linha é dividida por 5, ficando (0,1,2,3,4), e os valores
 * 1 e 3 serão as cordenadas de inicio e fim para a criação
 * da linha.
 */
void WINPIXELCALL wpx_line_gap (int x, int y, int w, int h, Color32 color) {

	vec2i m = {(x+w)/2, (y+h)/2};
	wpx_line((x+m.x)/2, (y+m.y)/2, (w+m.x)/2, (h+m.y)/2, color);
}

void WINPIXELCALL wpx_line_dash (int x, int y, int w, int h, float dash, Color32 color) {

	float dis = sqrtf((w-x)*(w-x)+(h-y)*(h-y))/fmaxf(dash,1);
	vec2i a = {(x-w)/dis, (y-h)/dis};
	wpx_line((x-a.x), (y-a.y), (w+a.x), (h+a.y), color);
}

void WINPIXELCALL wpx_line_dashed (int x, int y, int w, int h, float dash, Color32 color) {

	float dis = distance_point(x, y, w, h);
	vec2i a = {0, 0};

	for (int i = 0; i < roundf(dis/fmaxf(dash,1)) + 1; i++) {

		float vel = normalize(i, 0.0f, (dis/fmaxf(dash,1)));
		vec2i b = {lerp(x, w, vel), lerp(y, h, vel)};
		if (i)
			wpx_line_gap(a.x, a.y, b.x, b.y, color);
		a = (vec2i) {b.x, b.y};
	}
}

void WINPIXELCALL wpx_rect (int x, int y, int w, int h, Color32 color) {

	wpx_line(x,   y, w+x,   y, color);
	wpx_line(x, y+h, w+x, h+y, color);
	wpx_line(x,   y,   x, h+y, color);
	wpx_line(x+w, y, x+w, h+y, color);
}

void WINPIXELCALL wpx_rect_center (int x, int y, int w, int h, Color32 color) {

	wpx_line(x-(w/2), y-(h/2), x+(w/2), y-(h/2), color);
	wpx_line(x-(w/2), y+(h/2), x+(w/2), y+(h/2), color);
	wpx_line(x-(w/2), y-(h/2), x-(w/2), y+(h/2), color);
	wpx_line(x+(w/2), y+(h/2), x+(w/2), y-(h/2), color);
}

void WINPIXELCALL wpx_rect_fill (int x, int y, int w, int h, Color32 color) {

	if (w < 0) { x += w; w = -w; }
	if (h < 0) { y += h; h = -h; }
	Color32 px = wpx_color_to_pixel(color);
	for (int j = y; j < y + h; j++)
		_span_fill(x, x + w, j, px);
}

void WINPIXELCALL wpx_rect_fill_grid (int x, int y, int w, int h, Color32 color) {

	if (w < 0) { x += w; w = -w; }
	if (h < 0) { y += h; h = -h; }
	Color32 px = wpx_color_to_pixel(color);
	for (int j = y; j < y + h; j++)
		_span_fill_grid(x, x + w - 1, j, px);
}

void WINPIXELCALL wpx_circle (int x0, int y0, int radius, Color32 color) {

	int x = radius-1;
	int y = 0;
	int dx = 1;
	int dy = 1;
	int err = dx - (radius << 1);

	while (x >= y) {
		wpx_pixel(x0 + x, y0 + y, color);
		wpx_pixel(x0 + y, y0 + x, color);
		wpx_pixel(x0 - y, y0 + x, color);
		wpx_pixel(x0 - x, y0 + y, color);
		wpx_pixel(x0 - x, y0 - y, color);
		wpx_pixel(x0 - y, y0 - x, color);
		wpx_pixel(x0 + y, y0 - x, color);
		wpx_pixel(x0 + x, y0 - y, color);

		if (err <= 0) {
			y++;
			err += dy;
			dy += 2;
		}

		if (err > 0) {
			x--;
			dx += 2;
			err += dx - (radius << 1);
		}
	}
}

WINPIXELDLL void WINPIXELCALL wpx_circle_fill (int xc, int yc, int radius, Color32 col) {

    if (!radius) return;
    Color32 px = wpx_color_to_pixel(col);
    int x = 0, y = radius, p = 3 - 2 * radius;

    while (y >= x) {
        _span_fill(xc - x, xc + x, yc - y, px);
        _span_fill(xc - y, xc + y, yc - x, px);
        _span_fill(xc - x, xc + x, yc + y, px);
        _span_fill(xc - y, xc + y, yc + x, px);
        if (p < 0)
            p += 4 * x++ + 6;
        else
            p += 4 * (x++ - y--) + 10;
    }
}

WINPIXELDLL void WINPIXELCALL wpx_triangle_fill(vec2i *triangle, Color32 col) {
    Color32 px = wpx_color_to_pixel(col);

    vec2i v[3] = { triangle[0], triangle[1], triangle[2] };
    if (v[0].y > v[1].y) { vec2i t = v[0]; v[0] = v[1]; v[1] = t; }
    if (v[0].y > v[2].y) { vec2i t = v[0]; v[0] = v[2]; v[2] = t; }
    if (v[1].y > v[2].y) { vec2i t = v[1]; v[1] = v[2]; v[2] = t; }

    int y0 = v[0].y, y1 = v[1].y, y2 = v[2].y;
    if (y0 == y2) return;

    float slope02 = (float)(v[2].x - v[0].x) / (float)(y2 - y0);

    if (y0 < y1) {
        float slope01 = (float)(v[1].x - v[0].x) / (float)(y1 - y0);
        float xa = (float)v[0].x, xb = (float)v[0].x;
        for (int y = y0; y < y1; y++, xa += slope02, xb += slope01)
            _span_fill((int)xa, (int)xb, y, px);
    }

    if (y1 < y2) {
        float slope12 = (float)(v[2].x - v[1].x) / (float)(y2 - y1);
        float xa = (float)v[0].x + slope02 * (float)(y1 - y0);
        float xb = (float)v[1].x;
        for (int y = y1; y <= y2; y++, xa += slope02, xb += slope12)
            _span_fill((int)xa, (int)xb, y, px);
    }
}

/* DEFINITION: SCREEN-DOOR TRANSPARENCY, ORDERED DITHERING */
WINPIXELDLL void WINPIXELCALL wpx_circle_fill_grid (int xc, int yc, int radius, Color32 col) {

    if (!radius) return;
    Color32 px = wpx_color_to_pixel(col);
    int x = 0, y = radius, p = 3 - 2 * radius;

    while (y >= x) {
        _span_fill_grid(xc - x, xc + x, yc - y, px);
        _span_fill_grid(xc - y, xc + y, yc - x, px);
        _span_fill_grid(xc - x, xc + x, yc + y, px);
        _span_fill_grid(xc - y, xc + y, yc + x, px);
        if (p < 0)
            p += 4 * x++ + 6;
        else
            p += 4 * (x++ - y--) + 10;
    }
}

/* DEFINITION: SCREEN-DOOR TRANSPARENCY, ORDERED DITHERING */
WINPIXELDLL void WINPIXELCALL wpx_triangle_fill_grid(vec2i *triangle, Color32 col) {
    Color32 px = wpx_color_to_pixel(col);

    vec2i v[3] = { triangle[0], triangle[1], triangle[2] };
    if (v[0].y > v[1].y) { vec2i t = v[0]; v[0] = v[1]; v[1] = t; }
    if (v[0].y > v[2].y) { vec2i t = v[0]; v[0] = v[2]; v[2] = t; }
    if (v[1].y > v[2].y) { vec2i t = v[1]; v[1] = v[2]; v[2] = t; }

    int y0 = v[0].y, y1 = v[1].y, y2 = v[2].y;
    if (y0 == y2) return;

    float slope02 = (float)(v[2].x - v[0].x) / (float)(y2 - y0);

    if (y0 < y1) {
        float slope01 = (float)(v[1].x - v[0].x) / (float)(y1 - y0);
        float xa = (float)v[0].x, xb = (float)v[0].x;
        for (int y = y0; y < y1; y++, xa += slope02, xb += slope01)
            _span_fill_grid((int)xa, (int)xb, y, px);
    }

    if (y1 < y2) {
        float slope12 = (float)(v[2].x - v[1].x) / (float)(y2 - y1);
        float xa = (float)v[0].x + slope02 * (float)(y1 - y0);
        float xb = (float)v[1].x;
        for (int y = y1; y <= y2; y++, xa += slope02, xb += slope12)
            _span_fill_grid((int)xa, (int)xb, y, px);
    }
}

void WINPIXELCALL wpx_triangle_fill_ex (
	vec2f    *triangle,
	float     x,
	float     y,
	float     scale,
	float     ang,
	Color32 col) {

	vec2f v[3];

	for (int i = 0; i < 3; i++) {
		/* copy */
		v[i] = triangle[i];
		/* rotate */
		v[i].x = triangle[i].x * cosf(radsf(ang)) - triangle[i].y * sinf(radsf(ang));
		v[i].y = triangle[i].x * sinf(radsf(ang)) + triangle[i].y * cosf(radsf(ang));
		/* scale */
		v[i].x *= scale;
		v[i].y *= scale;
		/* translate */
		v[i].x += x;
		v[i].y += y;
	}

	vec2i v2[3] = {
		{v[0].x, v[0].y},
		{v[1].x, v[1].y},
		{v[2].x, v[2].y}
	};
	wpx_triangle_fill(v2, col);
}

/* fast elipse */
void WINPIXELCALL wpx_ellipse (int x, int y, int radius_x, int radius_y, Color32 color) {

	int ix, iy;
	int h, i, j, k;
	int oh, oi, oj, ok;
	int xmh, xph, ypk, ymk;
	int xmi, xpi, ymj, ypj;
	int xmj, xpj, ymi, ypi;
	int xmk, xpk, ymh, yph;

	/*
	* Sanity check radii 
	*/
	if ((radius_x < 0) || (radius_y < 0))
		return;

	/*
	* Special case for radius_x=0 - draw a vline 
	*/
	if (radius_x == 0) {
		wpx_line(x, y - radius_y, x, y + radius_y, color);
		return;
		// return (vlineRGBA(skne_render, x, y - radius_y, y + radius_y, c.r, c.g, c.b, c.a));
	}
	/*
	* Special case for radius_y=0 - draw a hline 
	*/
	if (radius_y == 0) {
		wpx_line(x - radius_x, y, x + radius_x, y, color);
		return;
		// return (hlineRGBA(skne_render, x - radius_x, x + radius_x, y, c.r, c.g, c.b, c.a));
	}

	/*
	* Init vars 
	*/
	oh = oi = oj = ok = 0xFFFF;

	/*
	* Draw 
	*/
	if (radius_x > radius_y) {
		ix = 0;
		iy = radius_x * 64;

		do {
			h = (ix + 32) >> 6;
			i = (iy + 32) >> 6;
			j = (h * radius_y) / radius_x;
			k = (i * radius_y) / radius_x;

			if (((ok != k) && (oj != k)) || ((oj != j) && (ok != j)) || (k != j)) {
				xph = x + h;
				xmh = x - h;
				if (k > 0) {
					ypk = y + k;
					ymk = y - k;
					wpx_pixel(xmh, ypk, color);
					wpx_pixel(xph, ypk, color);
					wpx_pixel(xmh, ymk, color);
					wpx_pixel(xph, ymk, color);
				} else {
					wpx_pixel(xmh, y, color);
					wpx_pixel(xph, y, color);
				}
				ok = k;
				xpi = x + i;
				xmi = x - i;
				if (j > 0) {
					ypj = y + j;
					ymj = y - j;
					wpx_pixel(xmi, ypj, color);
					wpx_pixel(xpi, ypj, color);
					wpx_pixel(xmi, ymj, color);
					wpx_pixel(xpi, ymj, color);
				} else {
					wpx_pixel(xmi, y, color);
					wpx_pixel(xpi, y, color);
				}
				oj = j;
			}

			ix = ix + iy / radius_x;
			iy = iy - ix / radius_x;

		} while (i > h);
	} else {
		ix = 0;
		iy = radius_y * 64;

		do {
			h = (ix + 32) >> 6;
			i = (iy + 32) >> 6;
			j = (h * radius_x) / radius_y;
			k = (i * radius_x) / radius_y;

			if (((oi != i) && (oh != i)) || ((oh != h) && (oi != h) && (i != h))) {
				xmj = x - j;
				xpj = x + j;
				if (i > 0) {
					ypi = y + i;
					ymi = y - i;
					wpx_pixel(xmj, ypi, color);
					wpx_pixel(xpj, ypi, color);
					wpx_pixel(xmj, ymi, color);
					wpx_pixel(xpj, ymi, color);
				} else {
					wpx_pixel(xmj, y, color);
					wpx_pixel(xpj, y, color);
				}
				oi = i;
				xmk = x - k;
				xpk = x + k;
				if (h > 0) {
					yph = y + h;
					ymh = y - h;
					wpx_pixel(xmk, yph, color);
					wpx_pixel(xpk, yph, color);
					wpx_pixel(xmk, ymh, color);
					wpx_pixel(xpk, ymh, color);
				} else {
					wpx_pixel(xmk, y, color);
					wpx_pixel(xpk, y, color);
				}
				oh = h;
			}

			ix = ix + iy / radius_y;
			iy = iy - ix / radius_y;

		} while (i > h);
	}

	return;
}

#define SPLINE_SEGMENT_DIVISIONS 20

/***************************************
 * Cubic easing in-out                 *
 * NOTE: Used by DrawLineBezier() only *
 ***************************************/
static float wpx_ease_cubic_in_out (float t, float b, float c, float d) {

	float result = 0.0f;

	if ((t /= 0.5f*d) < 1)
		result = 0.5f*c*t*t*t + b;
	else {
		t -= 2;
		result = 0.5f*c*(t*t*t + 2.0f) + b;
	}

	return result;
}

// Draw line using cubic-bezier spline, in-out interpolation, no control points
void WINPIXELCALL wpx_bezier_thick (
	vec2f    startPos,
	vec2f    endPos,
	float    thick,
	Color32 color) {

	vec2f previous = startPos;
	vec2f current = {0};

	vec2f points[2*SPLINE_SEGMENT_DIVISIONS + 2] = {0};

	for (int i = 1; i <= SPLINE_SEGMENT_DIVISIONS; i++) {
		// Cubic easing in-out
		// NOTE: Easing is calculated only for y position value
		current.y = wpx_ease_cubic_in_out(
			(float)i,
			startPos.y,
			endPos.y - startPos.y,
			(float)SPLINE_SEGMENT_DIVISIONS
		);
		current.x = previous.x + (endPos.x - startPos.x)/(float)SPLINE_SEGMENT_DIVISIONS;

		float dy = current.y - previous.y;
		float dx = current.x - previous.x;
		float size = 0.5f*thick/sqrtf(dx*dx+dy*dy);

		if (i == 1) {
			points[0].x = previous.x + dy*size;
			points[0].y = previous.y - dx*size;
			points[1].x = previous.x - dy*size;
			points[1].y = previous.y + dx*size;
		}

		points[2*i + 1].x = current.x - dy*size;
		points[2*i + 1].y = current.y + dx*size;
		points[2*i].x = current.x + dy*size;
		points[2*i].y = current.y - dx*size;

		previous = current;
	}

	int lot = (2*SPLINE_SEGMENT_DIVISIONS + 2);

	/* draw closed polygon */
	for (int i = 0; i < (lot - 2); i++) {
		vec2i v[3] = {
			{points[i+0].x, points[i+0].y},
			{points[i+1].x, points[i+1].y},
			{points[i+2].x, points[i+2].y},
		};
		wpx_triangle_fill(v, color);
	}
}

/* Subtracts t from each RGB channel (clamped to 0), preserving alpha. */
static inline Color32 _color_sub(Color32 c, uint8_t t) {
    uint8_t r = (c >> 24) & 0xFF; r = r > t ? r - t : 0;
    uint8_t g = (c >> 16) & 0xFF; g = g > t ? g - t : 0;
    uint8_t b = (c >>  8) & 0xFF; b = b > t ? b - t : 0;
    return ((Color32)r << 24) | ((Color32)g << 16) | ((Color32)b << 8) | (c & 0xFF);
}

WINPIXELDLL void WINPIXELCALL wpx_timegraph (WPX_TimeGraph *g, float value,
    int x, int y, int w, int h, Color32 color) {

    if (value < 0.0f) value = 0.0f;
    if (value > 1.0f) value = 1.0f;

    g->buf[g->head % WPX_TGRAPH_CAP] = value;
    g->head++;
    if (g->len < WPX_TGRAPH_CAP) g->len++;

    wpx_rect_fill(x, y, w, h, 0x0D1117FF);

    Color32 grid = 0x1E2836FF;
    for (int q = 1; q < 4; q++)
        wpx_line(x + 1, y + h * q / 4, x + w - 2, y + h * q / 4, grid);

    int samples = g->len < w - 2 ? g->len : w - 2;
    int start   = g->head - samples;
    int prev_py = -1;

    for (int i = 0; i < samples; i++) {
        float v  = g->buf[(start + i + WPX_TGRAPH_CAP) % WPX_TGRAPH_CAP];
        int   px = x + 1 + (w - 2 - samples) + i;
        int   py = y + h - 2 - (int)(v * (h - 3));

        wpx_line(px, py + 1, px, y + h - 2, _color_sub(color, 0xBB));

        int y0 = prev_py < py ? prev_py : py;
        int y1 = prev_py < py ? py : prev_py;
        if (prev_py >= 0)
            for (int sy = y0; sy <= y1; sy++)
                wpx_pixel(px, sy, color);
        else
            wpx_pixel(px, py, color);

        prev_py = py;
    }

    wpx_rect(x, y, w, h, 0x30363DFF);
}

// Draw spline segment: Linear, 2 points
void WINPIXELCALL wpx_spline (
	float     x,
	float     y,
	float     w,
	float     h,
	float     thick,
	Color32   color) {
	// NOTE: For the linear spline we don't use subdivisions, just a single quad

	vec2f delta = {w - x, h - y};
	float length = sqrtf(delta.x*delta.x + delta.y*delta.y);

	if ((length > 0) && (thick > 0)) {
		float scale = thick/(2*length);

		vec2f radius = { -scale*delta.y, scale*delta.x };
		vec2f strip[4] = {
			{x - radius.x, y - radius.y},
			{x + radius.x, y + radius.y},
			{w - radius.x, h - radius.y},
			{w + radius.x, h + radius.y}
		};
		vec2i v1[3] = {
			{strip[0].x, strip[0].y},
			{strip[1].x, strip[1].y},
			{strip[2].x, strip[2].y},
		};
		vec2i v2[3] = {
			{strip[1].x, strip[1].y},
			{strip[2].x, strip[2].y},
			{strip[3].x, strip[3].y},
		};
		wpx_triangle_fill(v1, color);
		wpx_triangle_fill(v2, color);
	}
}

// Draw spline segment: Linear, 2 points
void WINPIXELCALL wpx_spline_grid (
	float     x,
	float     y,
	float     w,
	float     h,
	float     thick,
	Color32   color) {
	// NOTE: For the linear spline we don't use subdivisions, just a single quad

	vec2f delta = {w - x, h - y};
	float length = sqrtf(delta.x*delta.x + delta.y*delta.y);

	if ((length > 0) && (thick > 0)) {
		float scale = thick/(2*length);

		vec2f radius = { -scale*delta.y, scale*delta.x };
		vec2f strip[4] = {
			{x - radius.x, y - radius.y},
			{x + radius.x, y + radius.y},
			{w - radius.x, h - radius.y},
			{w + radius.x, h + radius.y}
		};
		vec2i v1[3] = {
			{strip[0].x, strip[0].y},
			{strip[1].x, strip[1].y},
			{strip[2].x, strip[2].y},
		};
		vec2i v2[3] = {
			{strip[1].x, strip[1].y},
			{strip[2].x, strip[2].y},
			{strip[3].x, strip[3].y},
		};
		wpx_triangle_fill_grid(v1, color);
		wpx_triangle_fill_grid(v2, color);
	}
}

void WINPIXELCALL wpx_spline_gap (
	int       x,
	int       y,
	int       w,
	int       h,
	float     thick,
	Color32   color) {

	vec2i m = {(x+w)/2, (y+h)/2};
	wpx_spline((x+m.x)/2, (y+m.y)/2, (w+m.x)/2, (h+m.y)/2, thick, color);
}

void WINPIXELCALL wpx_spline_dashed (
	int       x,
	int       y,
	int       w,
	int       h,
	float     thick,
	float     dash,
	Color32   color) {

	float dis = distance_point(x, y, w, h);
	vec2i a = {0, 0};

	for (int i = 0; i < roundf(dis/fmaxf(dash,1)) + 1; i++) {

		float vel = normalize(i, 0.0f, (dis/fmaxf(dash,1)));
		vec2i b = {lerp(x, w, vel), lerp(y, h, vel)};
		if (i)
			wpx_spline_gap(a.x, a.y, b.x, b.y, thick, color);
		a = (vec2i) {b.x, b.y};
	}
}


// Draw spline segment: Quadratic Bezier, 2 points, 1 control point
void wpx_spline_segment_bezier_quadratic (
	vec2f    p1,
	vec2f    c2,
	vec2f    p3,
	float    thick,
	Color32  color) {

	const float step = 1.0f/SPLINE_SEGMENT_DIVISIONS;

	vec2f previous = p1;
	vec2f current = { 0 };
	float t = 0.0f;

	vec2f points[2*SPLINE_SEGMENT_DIVISIONS + 2] = { 0 };

	for (int i = 1; i <= SPLINE_SEGMENT_DIVISIONS; i++) {
		t = step*(float)i;

		float mt = 1.0f - t;
		float a  = mt * mt;
		float b  = 2.0f * mt * t;
		float c  = t * t;

		// NOTE: The easing functions aren't suitable here because they don't take a control point
		current.y = a*p1.y + b*c2.y + c*p3.y;
		current.x = a*p1.x + b*c2.x + c*p3.x;

		float dy = current.y - previous.y;
		float dx = current.x - previous.x;
		float size = 0.5f*thick/sqrtf(dx*dx+dy*dy);

		if (i == 1) {
			points[0].x = previous.x + dy*size;
			points[0].y = previous.y - dx*size;
			points[1].x = previous.x - dy*size;
			points[1].y = previous.y + dx*size;
		}

		points[2*i + 1].x = current.x - dy*size;
		points[2*i + 1].y = current.y + dx*size;
		points[2*i].x = current.x + dy*size;
		points[2*i].y = current.y - dx*size;

		previous = current;
	}

	/* draw closed polygon */
	int lot = (2*SPLINE_SEGMENT_DIVISIONS + 2);
	for (int i = 0; i < (lot - 2); i++) {
		vec2i v[3] = {
			{points[i+0].x, points[i+0].y},
			{points[i+1].x, points[i+1].y},
			{points[i+2].x, points[i+2].y},
		};
		wpx_triangle_fill(v, color);
	}
	// DrawTriangleStrip(points, 2*SPLINE_SEGMENT_DIVISIONS + 2, color);
}

// Draw spline segment: Cubic Bezier, 2 points, 2 control points
void wpx_spline_segment_bezier_cubic (
	vec2f    p1,
	vec2f    c2,
	vec2f    c3,
	vec2f    p4,
	float    thick,
	Color32  color) {

	const float step = 1.0f/SPLINE_SEGMENT_DIVISIONS;

	vec2f previous = p1;
	vec2f current = { 0 };
	float t = 0.0f;

	vec2f points[2*SPLINE_SEGMENT_DIVISIONS + 2] = { 0 };

	for (int i = 1; i <= SPLINE_SEGMENT_DIVISIONS; i++) {
		t = step*(float)i;

		float mt  = 1.0f - t;
		float mt2 = mt * mt;
		float t2  = t * t;
		float a   = mt2 * mt;
		float b   = 3.0f * mt2 * t;
		float c   = 3.0f * mt  * t2;
		float d   = t2 * t;

		current.y = a*p1.y + b*c2.y + c*c3.y + d*p4.y;
		current.x = a*p1.x + b*c2.x + c*c3.x + d*p4.x;

		float dy = current.y - previous.y;
		float dx = current.x - previous.x;
		float size = 0.5f*thick/sqrtf(dx*dx+dy*dy);

		if (i == 1)
		{
			points[0].x = previous.x + dy*size;
			points[0].y = previous.y - dx*size;
			points[1].x = previous.x - dy*size;
			points[1].y = previous.y + dx*size;
		}

		points[2*i + 1].x = current.x - dy*size;
		points[2*i + 1].y = current.y + dx*size;
		points[2*i].x = current.x + dy*size;
		points[2*i].y = current.y - dx*size;

		previous = current;
	}

	/* draw closed polygon */
	int lot = (2*SPLINE_SEGMENT_DIVISIONS + 2);
	for (int i = 0; i < (lot - 2); i++) {
		vec2i v[3] = {
			{points[i+0].x, points[i+0].y},
			{points[i+1].x, points[i+1].y},
			{points[i+2].x, points[i+2].y},
		};
		wpx_triangle_fill(v, color);
	}
	// DrawTriangleStrip(points, 2*SPLINE_SEGMENT_DIVISIONS + 2, color);
}

// Draw spline: Quadratic Bezier, minimum 3 points (1 control point): [p1, c2, p3, c4...]
void WINPIXELCALL wpx_spline_bezier_quadratic (
	const vec2f *points,
	int          pointCount,
	float        thick,
	Color32      color) {

	if (pointCount >= 3) {
		for (int i = 0; i < pointCount - 2; i += 2)
			wpx_spline_segment_bezier_quadratic(
				points[i],
				points[i + 1],
				points[i + 2],
				thick,
				color
			);
		// Cap circle drawing at the end of every segment
		//for (int i = 2; i < pointCount - 2; i += 2) DrawCircleV(points[i], thick/2.0f, color);
	}
}

// Draw spline: Cubic Bezier, minimum 4 points (2 control points): [p1, c2, c3, p4, c5, c6...]
void WINPIXELCALL wpx_spline_bezier_cubic (
	const vec2f *points,
	int          pointCount,
	float        thick,
	Color32      color) {

	if (pointCount >= 4) {
		for (int i = 0; i < pointCount - 3; i += 3)
			wpx_spline_segment_bezier_cubic(
				points[i],
				points[i + 1],
				points[i + 2],
				points[i + 3],
				thick,
				color
			);
		// Cap circle drawing at the end of every segment
		//for (int i = 3; i < pointCount - 3; i += 3) DrawCircleV(points[i], thick/2.0f, color);
	}
}