#include <float.h>

#include <winpixel.h>

int WINPIXELCALL fequal (float a, float b) {

	return fabsf(a - b) < FLT_EPSILON;
}

char WINPIXELCALL *strfmt (char *buf, int64_t value) {

	int i, j = 0, k;
	char str[80];

	snprintf(str, sizeof str, "%llu", value);

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

	float result = value - (max - min)*floorf((value - min)/(max - min));

	return result;
}

float WINPIXELCALL clamp (float x, float lower, float upper) {

	return __min(upper, __max(x, lower));
}

float WINPIXELCALL distance_point (float x, float y, float x2, float y2) {

	return sqrtf(powf(x2 - x, 2) + powf(y2 - y, 2));
}

float WINPIXELCALL direction_point (float x, float y, float x2, float y2) {

	float dir = -atan2f((y2 - y), (x2 - x)) * (180.0f / M_PI);
	if (dir <   0.000000f) dir += 360.000000f;
	if (dir > 360.000000f) dir  =   0.000000f;
	return fabs(dir);
}

int WINPIXELCALL isprime (int n) {

	if (n <= 1 || (n % 2 == 0 && n > 2))
		return 0;
	for (int i = 3; i < n / 2; i+= 2)
		if (n % i == 0)
			return 0;
	return 1;
}

/* one_at_a_time */
uint32_t WINPIXELCALL hash (const char *str) {

	if (str == NULL)
		return 0x00000000;

	size_t i = 0;
	uint32_t hash = 0;
	while (i != strlen(str)) {
		hash += str[i++];
		hash += hash << 10;
		hash ^= hash >> 6;
	}
	hash += hash << 3;
	hash ^= hash >> 11;
	hash += hash << 15;
	return hash;
}

/* embaralhar lista de inteiros */
void WINPIXELCALL int_shuffle (int *array, int size) {

	for (int i = size - 1; i > 0; i--) {
		int j = rand() % (i + 1);
		int tmp  = array[j];
		array[j] = array[i];
		array[i] = tmp;
	}
}

int WINPIXELCALL line_in_point (rec4f line, vec2f point, float tolerance) {

	return (fabsf(
			 distance_point(line.x, line.y,  line.w,  line.h) -
			(distance_point(line.x, line.y, point.x, point.y) +
			 distance_point(line.w, line.h, point.x, point.y))) <=
			fmaxf(tolerance, 0.01f));
}

int WINPIXELCALL line_in_line (rec4f line1, rec4f line2) {

	float s1_x = (line1.w - line1.x);
	float s1_y = (line1.h - line1.y);
	float s2_x = (line2.w - line2.x);
	float s2_y = (line2.h - line2.y);
	float s = (-s1_y * (line1.x - line2.x) + s1_x *(line1.y - line2.y)) /
		(-s2_x * s1_y + s1_x * s2_y);
	float t = (s2_x * (line1.y - line2.y) - s2_y * (line1.x - line2.x)) /
		(-s2_x * s1_y + s1_x * s2_y);
	return s >= 0 && s <= 1 && t >= 0 && t <= 1;
}

/* Calcular o meio de linha */
vec2f WINPIXELCALL middle_line (float x, float y, float x2, float y2) {

	return (vec2f) {(x + x2) / 2.0f, (y + y2) / 2.0f};
}

int32_t WINPIXELCALL sqrt_int (int32_t value) {

	int8_t sign = 1;

	if (value < 0) {
		sign   = -1;
		value *= -1;
	}

	uint32_t result = 0;
	uint32_t a      = value;
	uint32_t b      = 1u << 30;

	while (b > a)
		b >>= 2;

	while (b != 0) {
		if (a >= result + b) {
			a -= result + b;
			result = result +  2 * b;
		}
		b      >>= 2;
		result >>= 1;
	}

	return result * sign;
}

// Inicializa o gerador
void WINPIXELCALL noise_generator (WPX_NOISEGEN *gen, float increment) {

	gen->currentX  = 0.0f;
	gen->increment = increment;
	gen->oldstep   = 0;
	gen->param_1   = random(255);
	gen->param_2   = random(255);
}


// Gera o próximo valor de ruído
float WINPIXELCALL noise (WPX_NOISEGEN *gen) {

	#define __SMOOTHSTEP(t) ((float)t * (float)t * (3 - 2 * (float)t))
	#define __GRADIENT(h) ((h&15)&8) ? (1.0f+((h&15)&7)) : -(1.0f+((h&15)&7))

	// Encontrar os pontos da grade que cercam x
	int step = (int) floor(gen->currentX);

	// Calcular o deslocamento dentro da célula
	float dx = gen->currentX - floor(gen->currentX);

	// Obter os hashes da tabela de permutação
	if (step != gen->oldstep) {
		gen->oldstep = step;
		gen->param_1 = gen->param_2;
		gen->param_2 = random(255);
	}

	// Obter os gradientes
	float g0 = __GRADIENT(gen->param_1);
	float g1 = __GRADIENT(gen->param_2);

	// Calcular as contribuições dos gradientes
	float v0 = g0 * dx;
	float v1 = g1 * (dx - 1.0f);

	// Interpolar os resultados
	float t = __SMOOTHSTEP(dx);
	float result = lerp(v0, v1, t) * 0.5f;

	// Incrementar a posição
	gen->currentX += gen->increment;
	if (gen->currentX > FLT_MAX)
		gen->currentX = 0.0f;

	#undef __SMOOTHSTEP
	#undef __GRADIENT

	return result;
}

// Reinicia o gerador
void WINPIXELCALL noise_reset (WPX_NOISEGEN *gen) {

	gen->currentX = 0.0f;
}

// Define novo incremento
void WINPIXELCALL noise_increment (WPX_NOISEGEN *gen, float newIncrement) {

	gen->increment = newIncrement;
}