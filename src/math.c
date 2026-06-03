#include "winpixel_internal.h"
#include <math.h>
#include <stdio.h>
#include <float.h>

int WINPIXELCALL fequal (float a, float b) {

	return fabsf(a - b) < FLT_EPSILON;
}

char WINPIXELCALL *strfmt (char *buf, int64_t value) {

	int i, j = 0, k;
	char str[80];

	snprintf(str, sizeof str, "%lld", value);

	for (i = 16, k = strlen(str + 1); i >= 0; i--)
		if ((j < 3) && (k >= 0)) {
			buf[i] = str[k];
			k--;
			j++;
		} else {
			buf[i] = '.';
			j = 0;
		}

	return buf;
}

const char WINPIXELCALL *vbytes (int64_t v) {

    static const char *units[] = {" B", "KB", "MB", "GB", "TB"};
    if (v <         1024LL) return units[0];
    if (v < (1024LL << 10)) return units[1];  // 1024 * 1024
    if (v < (1024LL << 20)) return units[2];  // 1024 * 1024 * 1024
    if (v < (1024LL << 30)) return units[3];  // 1024 * 1024 * 1024 * 1024
    return units[4]; /* Retornar marcador de Bytes */
}

float WINPIXELCALL lerp (float start, float end, float t) {

	return start * (1.0f - t) + end * t;
}

float WINPIXELCALL invlerp (float start, float end, float time) {

	return (time - start) / (end - start);
}

/* normalize(50, 0, 100) == 0.500000 */
float WINPIXELCALL normalize (float x, float min, float max) {

	return (x - min) / (max - min);
}

float WINPIXELCALL map (
	float value,
	float inputMin,
	float inputMax,
	float outputMin,
	float outputMax) {
	
	return (value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin;
}

/* loop: (value > max) = min, (value < min) = max */
float WINPIXELCALL wrap (float value, float min, float max) {

	return value - (max - min) * floorf((value - min) / (max - min));
}

float WINPIXELCALL clamp (float x, float lower, float upper) {

	return fminf(upper, fmaxf(x, lower));
}

float WINPIXELCALL distance_point (float x, float y, float x2, float y2) {

	float dx = x2 - x, dy = y2 - y;
	return sqrtf(dx*dx + dy*dy);
}

float WINPIXELCALL direction_point (float x, float y, float x2, float y2) {

	float dir = -atan2f((y2 - y), (x2 - x)) * (180.0f / M_PI);
	if (dir <   0.0f) dir += 360.0f;
	if (dir > 360.0f) dir  =   0.0f;
	return dir;
}

int WINPIXELCALL isprime (int number) {

	if (number <= 1 || (number % 2 == 0 && number > 2))
		return 0;
	for (int i = 3; i * i <= number; i += 2)
		if (number % i == 0)
			return 0;
	return 1;
}

/* one_at_a_time */
uint32_t WINPIXELCALL hash (const char *str) {

	if (str == NULL)
		return 0x00000000;

	size_t i = 0;
	uint32_t hash = 0;
	size_t len = strlen(str);
	while (i != len) {
		hash += str[i++];
		hash += hash << 10;
		hash ^= hash >> 6;
	}
	hash += hash << 3;
	hash ^= hash >> 11;
	hash += hash << 15;
	return hash;
}

/* Fisher-Yates shuffle for integer arrays */
void WINPIXELCALL int_shuffle (int *array, int size) {

	for (int i = size - 1; i > 0; i--) {
		int j = rand() % (i + 1);
		int tmp  = array[j];
		array[j] = array[i];
		array[i] = tmp;
	}
}

int WINPIXELCALL line_in_point (rectf line, vec2f point, float tolerance) {

	/* Project point onto segment, clamp to [0,1], compare squared distances. */
	float ax = line.w - line.x, ay = line.h - line.y;
	float px = point.x - line.x, py = point.y - line.y;
	float len2 = ax*ax + ay*ay;
	float t = (len2 > 0.0f) ? (px*ax + py*ay) / len2 : 0.0f;
	if (t < 0.0f) t = 0.0f;
	if (t > 1.0f) t = 1.0f;
	float dx = px - t*ax, dy = py - t*ay;
	float tol = fmaxf(tolerance, 0.01f);
	return dx*dx + dy*dy <= tol*tol;
}

int WINPIXELCALL line_in_line (rectf line1, rectf line2) {

	float s1_x = line1.w - line1.x, s1_y = line1.h - line1.y;
	float s2_x = line2.w - line2.x, s2_y = line2.h - line2.y;
	float denom = -s2_x * s1_y + s1_x * s2_y;
	if (denom == 0.0f) return 0;
	float s = (-s1_y * (line1.x - line2.x) + s1_x * (line1.y - line2.y)) / denom;
	float t = ( s2_x * (line1.y - line2.y) - s2_y * (line1.x - line2.x)) / denom;
	return s >= 0.0f && s <= 1.0f && t >= 0.0f && t <= 1.0f;
}

/* Returns the midpoint between two points */
vec2f WINPIXELCALL middle_line (float x, float y, float x2, float y2) {

	return (vec2f) {(x + x2) / 2.0f, (y + y2) / 2.0f};
}

void WINPIXELCALL noise_generator (Noisegen *gen, float increment) {

	gen->currentX  = 0.0f;
	gen->increment = increment;
	gen->oldstep   = 0;
	gen->param_1   = random(255);
	gen->param_2   = random(255);
}


float WINPIXELCALL noise (Noisegen *gen) {

	#define NOISE_SMOOTHSTEP(t) ((float)(t) * (float)(t) * (3 - 2 * (float)(t)))
	#define NOISE_GRADIENT(h)   ((h) & 1 ? 1.0f : -1.0f)

	float floored = floorf(gen->currentX);
	int   step    = (int) floored;
	float dx = gen->currentX - floored;

	if (step != gen->oldstep) {
		gen->oldstep = step;
		gen->param_1 = gen->param_2;
		gen->param_2 = random(255);
	}

	float g0 = NOISE_GRADIENT(gen->param_1);
	float g1 = NOISE_GRADIENT(gen->param_2);
	float v0 = g0 * dx;
	float v1 = g1 * (dx - 1.0f);
	float t      = NOISE_SMOOTHSTEP(dx);
	float result = lerp(v0, v1, t);

	gen->currentX += gen->increment;
	if (isinf(gen->currentX))
		gen->currentX = 0.0f;

	#undef NOISE_SMOOTHSTEP
	#undef NOISE_GRADIENT

	return result;
}

void WINPIXELCALL noise_reset (Noisegen *gen) {

	gen->currentX = 0.0f;
}

void WINPIXELCALL noise_increment (Noisegen *gen, float newIncrement) {

	gen->increment = newIncrement;
}