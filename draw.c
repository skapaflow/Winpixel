#include <winpixel.h>

#define swap(x,y) {int t = x; x = y; y = t;};
#define drawline(sx,ex,ny) {\
	for (int i = sx; i <= ex; i++) {\
		wpx_pixel(i, ny, col);\
	}\
}

wpx_RGB WINPIXELCALL hex_to_rgb (wpx_COLOR h) {

	return (wpx_RGB) {
		.r = h >> 16,
		.g = (h & 0x00ff00) >> 8,
		.b = (h & 0x0000ff)
	};
}

wpx_COLOR WINPIXELCALL rgb_to_hex (int r, int g, int b) {

	return ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);
}

void WINPIXELCALL wpx_pixel (int x, int y, wpx_COLOR color) {

	WINPIXEL w = wpx_render;
	if (w.draw_buffer != NULL)
		if ((x >= 0 && y >= 0) && (x < w.screen_w && y < w.screen_h))
			w.draw_buffer[WPXSET(w,x,y)] = color;
}

wpx_COLOR WINPIXELCALL wpx_getpixel (int x, int y) {

	WINPIXEL w = wpx_render;
	if (w.draw_buffer != NULL)
		if ((x >= 0 && y >= 0) && (x < w.screen_w && y < w.screen_h))
			return w.draw_buffer[WPXSET(w,x,y)];
	return 0x000000;
}

void WINPIXELCALL wpx_line (int x0, int y0, int x1, int y1, wpx_COLOR color) {

	int dx = abs(x1 - x0);
	int dy = abs(y1 - y0);
	int sx = (x0 < x1) ? 1 : -1;
	int sy = (y0 < y1) ? 1 : -1;
	int err = dx - dy;

	while (1) {

		wpx_pixel(x0, y0, color);

		if (x0 == x1 && y0 == y1)
			break;

		int e2 = 2 * err;
		if (e2 > -dy) {
			err -= dy;
			x0 += sx;
		}
		if (e2 < dx) {
			err += dx;
			y0 += sy;
		}
	}
}
/*
 * A linha é dividida por 5, ficando (0,1,2,3,4), e os valores
 * 1 e 3 serão as cordenadas de inicio e fim para a criação
 * da linha.
 */
void WINPIXELCALL wpx_line_gap (int x, int y, int w, int h, wpx_COLOR color) {

	vec2i m = {(x+w)/2, (y+h)/2};
	wpx_line((x+m.x)/2, (y+m.y)/2, (w+m.x)/2, (h+m.y)/2, color);
}

void WINPIXELCALL wpx_line_dash (int x, int y, int w, int h, float dash, wpx_COLOR color) {

	float dis = sqrtf((w-x)*(w-x)+(h-y)*(h-y))/fmaxf(dash,1);
	vec2i a = {(x-w)/dis, (y-h)/dis};
	wpx_line((x-a.x), (y-a.y), (w+a.x), (h+a.y), color);
}

void WINPIXELCALL wpx_line_dashed (int x, int y, int w, int h, float dash, wpx_COLOR color) {

	float dis = distance_point(x, y, w, h);

	for (int i = 0; i < roundf(dis/fmaxf(dash,1)) + 1; i++) {

		float vel = normalize(i, 0.0f, (dis/fmaxf(dash,1)));
		vec2i b = {lerp(x, w, vel), lerp(y, h, vel)};
	#if 0
		wpx_circle(wpx_render, b.x, b.y, 5, color);
	#endif
		static vec2i a = {0,0};
		if (i)
			wpx_line_gap(a.x, a.y, b.x, b.y, color);
		a = (vec2i) {b.x, b.y};
	}
}

void WINPIXELCALL wpx_rect (int x, int y, int w, int h, wpx_COLOR color) {

	wpx_line(x,   y, w+x,   y, color);
	wpx_line(x, y+h, w+x, h+y, color);
	wpx_line(x,   y,   x, h+y, color);
	wpx_line(x+w, y, x+w, h+y, color);
}

void WINPIXELCALL wpx_rect_center (int x, int y, int w, int h, wpx_COLOR color) {

	wpx_line(x-(w/2), y-(h/2), x+(w/2), y-(h/2), color);
	wpx_line(x-(w/2), y+(h/2), x+(w/2), y+(h/2), color);
	wpx_line(x-(w/2), y-(h/2), x-(w/2), y+(h/2), color);
	wpx_line(x+(w/2), y+(h/2), x+(w/2), y-(h/2), color);
}

void WINPIXELCALL wpx_rect_fill (int x, int y, int w, int h, wpx_COLOR color) {

	// Ajusta x e y de acordo com a direção de w e h
	if (w < 0) {
		x += w;
		w = -w;
	}
	if (h < 0) {
		y += h;
		h = -h;
	}
	int y_end = y + h;
	for (int current_y = y; current_y < y_end; current_y++)
		wpx_line(x, current_y, x + w, current_y, color);
}

void WINPIXELCALL wpx_circle (int x0, int y0, int radius, wpx_COLOR color) {

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

void WINPIXELCALL wpx_circle_fill (int xc, int yc, int radius, wpx_COLOR col) {

	int x = 0;
	int y = radius;
	int p = 3 - 2 * radius;

	if (!radius)
		return;

	while (y >= x) {
		drawline(xc - x, xc + x, yc - y);
		drawline(xc - y, xc + y, yc - x);
		drawline(xc - x, xc + x, yc + y);
		drawline(xc - y, xc + y, yc + x);
		if (p < 0)
			p += 4 * x++ + 6;
		else
			p += 4 * (x++ - y--) + 10;
	}
}

void WINPIXELCALL wpx_triangle_fill (vec2i *v, wpx_COLOR col) {

	int t1x, t2x, y, minx, maxx, t1xp, t2xp;
	int changed1 = 0;
	int changed2 = 0;
	int signx1, signx2, dx1, dy1, dx2, dy2;
	int e1, e2;

	/* sort vertices */
	if (v[0].y > v[1].y) {swap(v[0].y, v[1].y); swap(v[0].x, v[1].x);}
	if (v[0].y > v[2].y) {swap(v[0].y, v[2].y); swap(v[0].x, v[2].x);}
	if (v[1].y > v[2].y) {swap(v[1].y, v[2].y); swap(v[1].x, v[2].x);}

	/* starting points */
	t1x = t2x = v[0].x; y = v[0].y; 
	dx1 = (int) (v[1].x - v[0].x);

	if (dx1 < 0) {
		dx1 = -dx1;
		signx1 = -1;
	} else
		signx1 = 1;

	dy1 = (int) (v[1].y - v[0].y);
	dx2 = (int) (v[2].x - v[0].x);

	if (dx2 < 0) {
		dx2 = -dx2;
		signx2 = -1;
	} else
		signx2 = 1;

	dy2 = (int) (v[2].y - v[0].y);

	/* swap values */
	if (dy1 > dx1) {
		swap(dx1, dy1);
		changed1 = 1;
	}

	/* swap values */
	if (dy2 > dx2) {
		swap(dy2, dx2);
		changed2 = 1;
	}

	e2 = (int) (dx2 >> 1);

	/* flat top, just process the second half */
	if (v[0].y == v[1].y)
		goto next;

	e1 = (int) (dx1 >> 1);

	for (int i = 0; i < dx1;) {
		t1xp = 0; t2xp = 0;
		if (t1x < t2x) {
			minx = t1x;
			maxx = t2x;
		} else {
			minx = t2x;
			maxx = t1x;
		}
		/* process first line until y value is about to change */
		while (i < dx1) {
			i++;
			e1 += dy1;
			while (e1 >= dx1) {
				e1 -= dx1;
				if (changed1)
					t1xp = signx1;//t1x += signx1;
				else
					goto next1;
			}
			if (changed1)
				break;
			else
				t1x += signx1;
		}
		/* Move line */

	next1:

		/* process second line until y value is about to change */
		while (1) {
			e2 += dy2;
			while (e2 >= dx2) {
				e2 -= dx2;
				if (changed2)
					t2xp = signx2;//t2x += signx2;
				else
					goto next2;
			}
			if (changed2)
				break;
			else
				t2x += signx2;
		}

	next2:

		if (minx > t1x) minx = t1x;
		if (minx > t2x) minx = t2x;
		if (maxx < t1x) maxx = t1x;
		if (maxx < t2x) maxx = t2x;

		/* Draw line from min to max 
		 * points found on the y
		 * Now increase y
		 */
		drawline(minx, maxx, y);

		if (!changed1)
			t1x += signx1;
		t1x += t1xp;
		if (!changed2)
			t2x += signx2;
		t2x += t2xp;
		y += 1;
		if (y == v[1].y)
			break;
	}

next:

	/* second half */
	dx1 = (int) (v[2].x - v[1].x);

	if (dx1<0) {
		dx1 = -dx1;
		signx1 = -1;
	} else
		signx1 = 1;

	dy1 = (int) (v[2].y - v[1].y);
	t1x = v[1].x;

	if (dy1 > dx1) { /* swap values */
		swap(dy1, dx1);
		changed1 = 1;
	} else
		changed1 = 0;

	e1 = (int) (dx1 >> 1);

	for (int i = 0; i <= dx1; i++) {
		t1xp = 0; t2xp = 0;
		if (t1x < t2x) {
			minx = t1x;
			maxx = t2x;
		} else {
			minx = t2x;
			maxx = t1x;
		}
		/* process first line until y value is about to change */
		while (i < dx1) {
			e1 += dy1;
			while (e1 >= dx1) {
				e1 -= dx1;
				if (changed1) {
					t1xp = signx1;
					break;
					//t1x += signx1;
				} else
					goto next3;
			}
			if (changed1)
				break;
			else
				t1x += signx1;
			if (i < dx1)
				i++;
		}

	next3:

		/* process second line until y value is about to change */
		while (t2x != v[2].x) {
			e2 += dy2;
			while (e2 >= dx2) {
				e2 -= dx2;
				if (changed2)
					t2xp = signx2;
				else
					goto next4;
			}
			if (changed2)
				break;
			else
				t2x += signx2;
		}

	next4:

		if (minx > t1x) minx = t1x;
		if (minx > t2x) minx = t2x;
		if (maxx < t1x) maxx = t1x;
		if (maxx < t2x) maxx = t2x;

		drawline(minx, maxx, y);

		if (!changed1)
			t1x += signx1;
		t1x += t1xp;
		if (!changed2)
			t2x += signx2;
		t2x += t2xp;
		y += 1;
		if (y > v[2].y)
			return;
	}
}

void WINPIXELCALL wpx_triangle_fill_ex (
	vec2f    *tri,
	float     x,
	float     y,
	float     scale,
	float     ang,
	wpx_COLOR col) {

	vec2f v[3];

	for (int i = 0; i < 3; i++) {
		/* copy */
		v[i] = tri[i];
		/* rotate */
		v[i].x = tri[i].x * cosf(radsf(ang)) - tri[i].y * sinf(radsf(ang));
		v[i].y = tri[i].x * sinf(radsf(ang)) + tri[i].y * cosf(radsf(ang));
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
void WINPIXELCALL wpx_ellipse (int x, int y, int rx, int ry, wpx_COLOR color) {

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
	if ((rx < 0) || (ry < 0))
		return;

	/*
	* Special case for rx=0 - draw a vline 
	*/
	if (rx == 0) {
		wpx_line(x, y - ry, x, y + ry, color);
		return;
		// return (vlineRGBA(skne_render, x, y - ry, y + ry, c.r, c.g, c.b, c.a));
	}
	/*
	* Special case for ry=0 - draw a hline 
	*/
	if (ry == 0) {
		wpx_line(x - rx, y, x + rx, y, color);
		return;
		// return (hlineRGBA(skne_render, x - rx, x + rx, y, c.r, c.g, c.b, c.a));
	}

	/*
	* Init vars 
	*/
	oh = oi = oj = ok = 0xFFFF;

	/*
	* Draw 
	*/
	if (rx > ry) {
		ix = 0;
		iy = rx * 64;

		do {
			h = (ix + 32) >> 6;
			i = (iy + 32) >> 6;
			j = (h * ry) / rx;
			k = (i * ry) / rx;

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

			ix = ix + iy / rx;
			iy = iy - ix / rx;

		} while (i > h);
	} else {
		ix = 0;
		iy = ry * 64;

		do {
			h = (ix + 32) >> 6;
			i = (iy + 32) >> 6;
			j = (h * rx) / ry;
			k = (i * rx) / ry;

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

			ix = ix + iy / ry;
			iy = iy - ix / ry;

		} while (i > h);
	}

	return;
}

#define SPLINE_SEGMENT_DIVISIONS 20

/***************************************
 * Cubic easing in-out                 *
 * NOTE: Used by DrawLineBezier() only *
 ***************************************/
static float __ease_cubic_in_out__ (float t, float b, float c, float d) {

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
	uint32_t color) {

	vec2f previous = startPos;
	vec2f current = {0};

	vec2f points[2*SPLINE_SEGMENT_DIVISIONS + 2] = {0};

	for (int i = 1; i <= SPLINE_SEGMENT_DIVISIONS; i++) {
		// Cubic easing in-out
		// NOTE: Easing is calculated only for y position value
		current.y = __ease_cubic_in_out__(
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

// Draw spline segment: Linear, 2 points
void WINPIXELCALL wpx_spline (
	float     px,
	float     py,
	float     pw,
	float     ph,
	float     thick,
	wpx_COLOR color) {
	// NOTE: For the linear spline we don't use subdivisions, just a single quad

	vec2f delta = { pw - px, ph - py };
	float length = sqrtf(delta.x*delta.x + delta.y*delta.y);

	if ((length > 0) && (thick > 0)) {
		float scale = thick/(2*length);

		vec2f radius = { -scale*delta.y, scale*delta.x };
		vec2f strip[4] = {
			{ px - radius.x, py - radius.y },
			{ px + radius.x, py + radius.y },
			{ pw - radius.x, ph - radius.y },
			{ pw + radius.x, ph + radius.y }
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

// Draw spline segment: Quadratic Bezier, 2 points, 1 control point
void __wpx_spline_segment_bezier_quadratic__ (
	vec2f    p1,
	vec2f    c2,
	vec2f    p3,
	float    thick,
	uint32_t color) {

	const float step = 1.0f/SPLINE_SEGMENT_DIVISIONS;

	vec2f previous = p1;
	vec2f current = { 0 };
	float t = 0.0f;

	vec2f points[2*SPLINE_SEGMENT_DIVISIONS + 2] = { 0 };

	for (int i = 1; i <= SPLINE_SEGMENT_DIVISIONS; i++) {
		t = step*(float)i;

		float a = powf(1.0f - t, 2);
		float b = 2.0f*(1.0f - t)*t;
		float c = powf(t, 2);

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
void __wpx_spline_segment_bezier_cubic__ (
	vec2f    p1,
	vec2f    c2,
	vec2f    c3,
	vec2f    p4,
	float    thick,
	uint32_t color) {

	const float step = 1.0f/SPLINE_SEGMENT_DIVISIONS;

	vec2f previous = p1;
	vec2f current = { 0 };
	float t = 0.0f;

	vec2f points[2*SPLINE_SEGMENT_DIVISIONS + 2] = { 0 };

	for (int i = 1; i <= SPLINE_SEGMENT_DIVISIONS; i++) {
		t = step*(float)i;

		float a = powf(1.0f - t, 3);
		float b = 3.0f*powf(1.0f - t, 2)*t;
		float c = 3.0f*(1.0f - t)*powf(t, 2);
		float d = powf(t, 3);

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
	uint32_t     color) {

	if (pointCount >= 3) {
		for (int i = 0; i < pointCount - 2; i += 2)
			__wpx_spline_segment_bezier_quadratic__(
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
	uint32_t     color) {

	if (pointCount >= 4) {
		for (int i = 0; i < pointCount - 3; i += 3)
			__wpx_spline_segment_bezier_cubic__(
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