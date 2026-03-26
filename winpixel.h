#ifndef WINPIXEL_H
#define WINPIXEL_H

#ifdef WINPIXEL_DLL
	#define WINPIXELDLL __declspec(dllexport)
#else
	#define WINPIXELDLL __declspec(dllimport)
#endif

/* Define calling convention in one place, for convenience. */
#define WINPIXELCALL __cdecl

#ifdef __cplusplus
extern "C"
{
#endif

#include <time.h>
#include <math.h>
#include <conio.h>
#include <stdio.h>
#include <stdint.h>
#include <malloc.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#include <windows.h>

#undef near
#undef far

#ifndef M_PI
	#define M_PI 3.14159265358979323846 /* ou outra precisão necessária */
#endif

#define __WINPIXEL

#define WPX16   16
#define WPX32   32
#define WPX64   64
#define WPX128  128
#define WPX256  256
#define WPX512  512
#define WPX1024 1024

#define WPXMIN 128
#define WPXMID 256
#define WPXMAX 1024
#define WPXHIG 5000

#define LIGHTRED   0xff6060
#define RED        0xFF0000
#define DARKRED    0x800000
#define PINK       0xFF80FF
#define MAGENTA    0xFF00FF
#define PURPLE     0x400080
#define SKYBLUE    0x0080FF
#define BLUE       0x0000FF
#define DARKBLUE   0x000080
#define CYAN       0x00FFFF
#define DARKCYAN   0x008080
#define PETROBLUE  0x084d6e
#define BLUEPRINT  0x19294E
#define LIME       0x00FF00
#define GREEN      0x008000
#define DARKGREEN  0x004000
#define YELLOW     0xFFFF00
#define DARKYELLOW 0x808000
#define ORANGE     0xff8000
#define BRAWN      0x804000
#define BLACK      0x000000
#define WHITE      0xFFFFFF
#define GRAY       0xC0C0C0
#define DARKGRAY   0x808080

#define WPX_BYTES          999	/* 1 byte - byte     */
#define WPX_KB          999999	/* 2 byte - Kilobyte */
#define WPX_MB       999999999	/* 3 byte - megabyte */
#define WPX_GB    999999999999	/* 4 byte - gigabyte */
#define WPX_TB 999999999999999	/* 5 byte - terabyte */

#define WPX_ASCII_LEN 96
#define WPX_ASCII_SIZE 36

#define M_TAU 6.28318530717958647692

#define WPX_DIV(a,b) ((b) != 0 ? ((a)/(b)): (int) 0) /* return ZERO */

#define WRGB(r,g,b) (((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff)) /* RGB to HEXA */
#define WRGBA(r,g,b,a) (((r & 0xff) << 24) + ((g & 0xff) << 16) + ((b & 0xff) << 8) + (a & 0xff)) /* RGBA to HEXA */
/*
 * ARGB(v)        Converter para (Alpha_RGB) se o canal for RGBA.
 * RGBA(v)        Converter para (RGB_Alpha) se o canal for ARGB.
 * ABGR_RGBA(v)   Inverter de (Alpha_BGR) para (RGB_Alpha).
 * RGB_6(v)       Converter para RGB sem Alpha (0xFFFFFF) se o canal for RGBA.
 * RGB_8(v,h)     Adicionar valor Alpha v = 0xFFFFFF h = Alpha se o canal for RGB.
 * RGB_ALPHA(a,v) Converte para (Alpha_RGB) se (a) for TRUE, com a entrada sendo RGBA,
 *                ou se (a) for FALSE, retorna RGB
 * I_RGB(v)        Inverter (RGB) 0x0080AA para 0xFF7F55
 * I_RGBA(v)       Inverter (RGBA) 0x0080AAFF para 0xFF7F55FF sem modificar Alpha
 * I_RGBA_FF(v)    Inverter (RGBA) 0x0080AAFF para 0xFF7F55FF e ajusta Alpha para 0xFF
 * RGB_FF(v)       Pega RGB e adicionar Alpha com 0xFF
 * RGBA_FF(v)      Pega RGBA e ajusta Alpha para 0xFF
 */

/*
 * Use this method to invert each color
 * and maintain original alpha.
 *
 *     data[i] ^= 0x00FFFFFF;
 *
 * if you want to ignore original alpha,
 * and just make it opaque, you can
 * overwrite the alpha:
 *
 *     data[i] = (0xFF000000 | ~data[i]); 
 */

#define ARGB(v) (((v&0xFF)<<24)|(((v>>24)&0xFF)<<16)|(((v>>16)&0xFF)<<8)|((v>>8)&0xFF))
#define RGBA(v) ((((v>>16)&0xFF)<<24)|(((v>>8)&0xFF)<<16)|((v&0xFF)<<8)|((v>>24)&0xFF))
#define ABGR_RGBA(v) (((v&0xFF)<<24)|(((v>>8)&0xFF)<<16)|(((v>>16)&0xFF)<<8)|((v>>24)&0xFF))
#define RGB_6(v) ((((v>>24)&0xFF)<<16)|(((v>>16)&0xFF)<<8)|((v>>8)&0xFF))
#define RGB_8(v,h) ((((v>>16)&0xFF)<<24)|((v>>8)<<16)|((v&0xFF)<<8)|h)
#define RGB_ALPHA(a,v) ((a)?ARGB(v):v)
#define I_RGB(v) (((0xFF-((v>>16)&0xFF))<<16)|((0xFF-((v>>8)&0xFF))<<8)|(0xFF-(v&0xFF)))
#define I_RGBA(v) (((0xFF-((v>>24)&0xFF))<<24)|((0xFF-((v>>16)&0xFF))<<16)|((0xFF-((v>>8)&0xFF))<<8)|(0xFF-((v)&0xFF)))
#define I_RGBA_FF(v) (((0xFF-((v>>24)&0xFF))<<24)|((0xFF-((v>>16)&0xFF))<<16)|((0xFF-((v>>8)&0xFF))<<8)|(0xFF))
#define RGB_FF(v) RGB_8(v,0xFF)
#define RGBA_FF(v) ((((v>>24)&0xFF)<<24)|(((v>>16)&0xFF)<<16)|(((v>>8)&0xFF)<<8)|(0xFF))

/*
	NOTE: sem problemas com numeros negativos, porem apresenta
		problema de limite para numeros positivos muito grandes!
*/
#define WPXSET(d,x,y) (int) (abs(x) + (abs(y) * d.screen_w))
#define WPXSET2(d,x,y) (int) (abs(x) + (abs(y) * (d)))
#define WPXSWAP(t, x, y) do {t __wpxswap__ = x; x = y; y = __wpxswap__;} while (0)

#define WPX_APPSTARTING IDC_APPSTARTING /* Standard arrow and small hourglass */
#define WPX_ARROW       IDC_ARROW       /* Standard arrow */
#define WPX_CROSS       IDC_CROSS       /* Crosshair */
#define WPX_HAND        IDC_HAND        /* Hand */
#define WPX_HELP        IDC_HELP        /* Arrow and question mark */
#define WPX_IBEAM       IDC_IBEAM       /* I-beam */
#define WPX_ICON        IDC_ICON        /* Obsolete for applications marked version 4.0 or later. */
#define WPX_NO          IDC_NO          /* Slashed circle */
#define WPX_SIZE        IDC_SIZE        /* Obsolete for applications marked version 4.0 or later. Use IDC_SIZEALL. */
#define WPX_SIZEALL     IDC_SIZEALL     /* Four-pointed arrow pointing north, south, east, and west */
#define WPX_SIZENESW    IDC_SIZENESW    /* Double-pointed arrow pointing northeast and southwest */
#define WPX_SIZENS      IDC_SIZENS      /* Double-pointed arrow pointing north and south */
#define WPX_SIZENWSE    IDC_SIZENWSE    /* Double-pointed arrow pointing northwest and southeast */
#define WPX_SIZEWE      IDC_SIZEWE      /* Double-pointed arrow pointing west and east */
#define WPX_UPARROW     IDC_UPARROW     /* Vertical arrow */
#define WPX_WAIT        IDC_WAIT        /* Hourglass */

/* math */
#define degs(r) ((r) * (180.0 / M_PI))   /* double */
#define rads(d) ((d) * (M_PI / 180.0))   /* double */
#define degsf(r) ((r) * (180.0f / M_PI)) /* float  */
#define radsf(d) ((d) * (M_PI / 180.0f)) /* float  */
#define ispar(x) ((int)x % 2 == 0)
/* func */
#define rseed(...) srand(time(NULL)),rand()
#define random(n)  (rand()%((n<0?0:n)+1))
#define frandom(n) (((float)rand()/(float)(RAND_MAX))*(float)((n)+1))
#define drandom(n) (((double)rand()/(double)(RAND_MAX))*(double)((n)+1))
#define lrandom(a,b) ((a)+rand()%((b)+1-(a)))
/* NOTE: limit float random is not precise */
#define lfrandom(a,b) (((b)-(a))*((((float)rand())/(float)RAND_MAX))+(a)) /* NOTE: lfrandom(0.1f, 0.3f) == 0.1f, 0.2f */
#define ldrandom(a,b) (((b)-(a))*((((double)rand())/(double)RAND_MAX))+(a)) /* NOTE: ldrandom(0.1, 0.3) == 0.1, 0.2 */
/*******************************************
 * Função random que recebe um array.      *
 * Ex:                                     *
 *     arandom(1,2,3);                     *
 * - arandom retorna um elemento do array. *
 *******************************************/
static inline int __rand_array_int__(int arr[], int size) {return arr[rand() % size];}
#define arandom(...)\
    __rand_array_int__(\
    (int[]) {__VA_ARGS__},\
    sizeof((int[]) {__VA_ARGS__})\
    / sizeof((int[]) {__VA_ARGS__}[0]))

static inline float __rand_array_float__(float arr[], int size) {return arr[rand() % size];}
#define afrandom(...)\
    __rand_array_float__(\
    (float[]) {__VA_ARGS__},\
    sizeof((float[]) {__VA_ARGS__})\
    / sizeof((float[]) {__VA_ARGS__}[0]))

// #define fequal(a,b,epsilon) (fabs((float) (a) - (float) (b)) < (float) epsilon)
#define _wpx_type(t) _Generic((t), int: 1, float: 2, char: 3, default: 0)
/* _malloc NEW */
#define wpx_malloc(v,n) wpx_alloc(v, malloc(sizeof(*v) * (n)), "malloc")
#define wpx_calloc(v,n) wpx_alloc(v, calloc(n, sizeof(*v)), "calloc")
#define wpx_realloc(v,n) wpx_alloc(v ,realloc(v, sizeof(*v) * (n)), "realloc")
#define wpx_free(f) if (f != NULL) free(f), f = NULL
#define wpx_fclose(f) if (f != NULL) fclose(f), f = NULL
#define wpx_memset(v,i,n) memset(v, i, sizeof(*v) * (n))
#define wpx_alloc(v,m,s) \
	if (((v) = m) == NULL) {\
		printf("\n [!] error_"s":\"%s\":%d\n", __FILE__, __LINE__);\
		getchar();\
		exit(EXIT_FAILURE);\
	}

/* vector 2 */
#define VEC2I (vec2i) {   0,    0}
#define VEC2F (vec2f) {0.0f, 0.0f}
#define VEC2D (vec2d) {0.0f, 0.0f}
typedef struct {int    x, y;} vec2i;
typedef struct {float  x, y;} vec2f;
typedef struct {double x, y;} vec2d;
/* vector 3 */
#define VEC3I (vec3i) {   0,    0,    0}
#define VEC3F (vec3f) {0.0f, 0.0f, 0.0f}
#define VEC3D (vec3d) {0.0f, 0.0f, 0.0f}
typedef struct {int    x, y, z;} vec3i;
typedef struct {float  x, y, z;} vec3f;
typedef struct {double x, y, z;} vec3d;
/* vector 4 */
#define VEC4I (vec4i) {   0,    0,    0,    0}
#define VEC4F (vec4f) {0.0f, 0.0f, 0.0f, 0.0f}
#define VEC4D (vec4d) {0.0f, 0.0f, 0.0f, 0.0f}
typedef struct {int    x, y, z, w;} vec4i;
typedef struct {float  x, y, z, w;} vec4f;
typedef struct {double x, y, z, w;} vec4d;
/* rect 4 */
#define REC4I (rec4i) {   0,    0,    0,    0}
#define REC4F (rec4f) {0.0f, 0.0f, 0.0f, 0.0f}
#define REC4D (rec4d) {0.0f, 0.0f, 0.0f, 0.0f}
typedef struct {int    x, y, w, h;} rec4i;
typedef struct {float  x, y, w, h;} rec4f;
typedef struct {double x, y, w, h;} rec4d;
/* options 2 */
#define VAR2I (var2i) {   0,    0}
#define VAR2F (var2f) {0.0f, 0.0f}
#define VAR2D (var2d) {0.0f, 0.0f}
typedef struct {int    a, b;} var2i;
typedef struct {float  a, b;} var2f;
typedef struct {double a, b;} var2d;
/* options 3 */
#define VAR3I (var3i) {   0,    0,    0}
#define VAR3F (var3f) {0.0f, 0.0f, 0.0f}
#define VAR3D (var3d) {0.0f, 0.0f, 0.0f}
typedef struct {int    a, b, c;} var3i;
typedef struct {float  a, b, c;} var3f;
typedef struct {double a, b, c;} var3d;
/* options 4 */
#define VAR4I (var4i) {   0,    0,    0,    0}
#define VAR4F (var4f) {0.0f, 0.0f, 0.0f, 0.0f}
#define VAR4D (var4d) {0.0f, 0.0f, 0.0f, 0.0f}
typedef struct {int    a, b, c, d;} var4i;
typedef struct {float  a, b, c, d;} var4f;
typedef struct {double a, b, c, d;} var4d;
/* vector 2 in/out */
#define VECIOI (vecioi) {   0,    0}
#define VECIOF (veciof) {0.0f, 0.0f}
#define VECIOD (veciod) {0.0f, 0.0f}
typedef struct {int in, out;} vecioi;
typedef struct {float in, out;} veciof;
typedef struct {double in, out;} veciod;

/* color */
typedef uint32_t wpx_COLOR;
typedef struct {float r, g, b;} wpx_RGB;
typedef struct {float r, g, b, a;} wpx_RGBA;

typedef struct __winpixel__ {
	int x, y;                 /* window position */
	int w, h;                 /* window size */
	int gap_w,     gap_h;     /* window bord size */
	int screen_w,  screen_h;  /* (REMOVER) windows size */
	int desktop_w, desktop_h; /* desktop size */
	/* draw */
	wpx_COLOR *draw_buffer;
	BITMAPINFO bitmap;
	/* windows handlers */
	HDC      screen_dc;
	char     win_id[32];
	long     screen_pitch;
	HBITMAP  screen_bmp;
	HBITMAP  screen_ob;
	HWND     screen_handle;
	uint8_t *screen_bf;
	// HCURSOR hcursor;
} WINPIXEL;

// Estrutura para o gerador de ruído
typedef struct {
	int   oldstep;
	int   param_1;
	int   param_2;
	float currentX;
	float increment;
} WPX_NOISEGEN;

extern WINPIXELDLL WINPIXEL wpx_render;
extern WINPIXELDLL vec2i wpx_mouse;
extern WINPIXELDLL float wpx_fps;

extern WINPIXELDLL int *KEY;
extern WINPIXELDLL int *KEYP;
extern WINPIXELDLL int *KEYR;

extern WINPIXELDLL int WPX_W, WPX_H;
extern WINPIXELDLL int WPX_W2, WPX_H2; /* dividido por 2 */
extern WINPIXELDLL int wpx_exit;
extern WINPIXELDLL int wpx_mouse_left_nclicks;
extern WINPIXELDLL int wpx_mouse_right_nclicks;
extern WINPIXELDLL int wpx_mouse_wheel;
extern WINPIXELDLL int wpx_mouse_wheel_down;
extern WINPIXELDLL int wpx_mouse_wheel_press;
extern WINPIXELDLL uint32_t wpx_mouse_left_down;
extern WINPIXELDLL uint32_t wpx_mouse_left_press;
extern WINPIXELDLL uint32_t wpx_mouse_left_up;
extern WINPIXELDLL uint32_t wpx_mouse_right_down;
extern WINPIXELDLL uint32_t wpx_mouse_right_press;
extern WINPIXELDLL uint32_t wpx_mouse_right_up;
extern WINPIXELDLL int wpx_keydown;
extern WINPIXELDLL int wpx_keypress;
extern WINPIXELDLL char wpx_keychar[2];
extern WINPIXELDLL char *wpx_dropfile;
/* window */
WINPIXELDLL int WINPIXELCALL winpixel_window (const char *, int, int);
WINPIXELDLL void WINPIXELCALL winpixel_maximize (void);
WINPIXELDLL void WINPIXELCALL winpixel_render (wpx_COLOR, int);
WINPIXELDLL void WINPIXELCALL winpixel_close (void);
/* input */
WINPIXELDLL int WINPIXELCALL key (int);
WINPIXELDLL int WINPIXELCALL key_press (int);
WINPIXELDLL int WINPIXELCALL key_release (int);
/* primitives */
WINPIXELDLL wpx_RGB WINPIXELCALL hex_to_rgb (wpx_COLOR);
WINPIXELDLL wpx_COLOR WINPIXELCALL rgb_to_hex (int, int, int);
WINPIXELDLL void WINPIXELCALL wpx_pixel (int, int, wpx_COLOR);
WINPIXELDLL wpx_COLOR WINPIXELCALL wpx_getpixel (int, int);
WINPIXELDLL void WINPIXELCALL wpx_line (int, int, int, int, wpx_COLOR);
WINPIXELDLL void WINPIXELCALL wpx_line_gap (int, int, int, int, wpx_COLOR);
WINPIXELDLL void WINPIXELCALL wpx_line_dash (int, int, int, int, float, wpx_COLOR);
WINPIXELDLL void WINPIXELCALL wpx_line_dashed (int, int, int, int, float, wpx_COLOR);
WINPIXELDLL void WINPIXELCALL wpx_rect (int, int, int, int, wpx_COLOR);
WINPIXELDLL void WINPIXELCALL wpx_rect_center (int, int, int, int, wpx_COLOR);
WINPIXELDLL void WINPIXELCALL wpx_rect_fill (int, int, int, int, wpx_COLOR);
WINPIXELDLL void WINPIXELCALL wpx_circle (int, int, int, wpx_COLOR);
WINPIXELDLL void WINPIXELCALL wpx_circle_fill (int, int, int, wpx_COLOR);
WINPIXELDLL void WINPIXELCALL wpx_triangle_fill (vec2i *, wpx_COLOR);
WINPIXELDLL void WINPIXELCALL wpx_triangle_fill_ex (vec2f *, float, float, float, float, wpx_COLOR);
WINPIXELDLL void WINPIXELCALL wpx_ellipse (int, int, int, int, wpx_COLOR);
WINPIXELDLL void WINPIXELCALL wpx_bezier_thick (vec2f, vec2f, float, wpx_COLOR);
WINPIXELDLL void WINPIXELCALL wpx_spline (float, float, float, float, float, wpx_COLOR);
WINPIXELDLL void WINPIXELCALL wpx_spline_gap (int, int, int, int, float, wpx_COLOR);
WINPIXELDLL void WINPIXELCALL wpx_spline_dashed (int, int, int, int, float, float, wpx_COLOR);
WINPIXELDLL void WINPIXELCALL wpx_spline_bezier_quadratic (const vec2f *, int, float, wpx_COLOR);
WINPIXELDLL void WINPIXELCALL wpx_spline_bezier_cubic (const vec2f *, int, float, wpx_COLOR);
/* math */
WINPIXELDLL int WINPIXELCALL fequal (float, float);
WINPIXELDLL char WINPIXELCALL *strfmt (char *, int64_t);
WINPIXELDLL const char WINPIXELCALL *vbytes (int64_t);
WINPIXELDLL float WINPIXELCALL lerp (float, float, float);
WINPIXELDLL float WINPIXELCALL invlerp (float, float, float);
WINPIXELDLL float WINPIXELCALL normalize (float, float, float);
WINPIXELDLL float WINPIXELCALL map (float, float, float, float, float);
WINPIXELDLL float WINPIXELCALL wrap (float, float, float);
WINPIXELDLL float WINPIXELCALL clamp (float, float, float);
WINPIXELDLL float WINPIXELCALL distance_point (float, float, float, float);
WINPIXELDLL float WINPIXELCALL direction_point (float, float, float, float);
WINPIXELDLL int WINPIXELCALL isprime (int);
WINPIXELDLL uint32_t WINPIXELCALL hash (const char *);
WINPIXELDLL void WINPIXELCALL int_shuffle (int *, int);
WINPIXELDLL int WINPIXELCALL line_in_point (rec4f, vec2f, float);
WINPIXELDLL int WINPIXELCALL line_in_line (rec4f, rec4f);
WINPIXELDLL vec2f WINPIXELCALL middle_line (float, float, float, float);
WINPIXELDLL int32_t WINPIXELCALL sqrt_int (int32_t);
WINPIXELDLL void WINPIXELCALL noise_generator (WPX_NOISEGEN *, float);
WINPIXELDLL float WINPIXELCALL noise (WPX_NOISEGEN *);
WINPIXELDLL void WINPIXELCALL noise_reset (WPX_NOISEGEN *);
WINPIXELDLL void WINPIXELCALL noise_increment (WPX_NOISEGEN *, float);
/* output */
WINPIXELDLL void WINPIXELCALL wpx_text_ex (int, wpx_COLOR, int, wpx_COLOR, int, int, int, const char *, ...);
WINPIXELDLL void WINPIXELCALL wpx_text_center (wpx_COLOR, int, int, int, const char *, ...);
WINPIXELDLL void WINPIXELCALL wpx_text (wpx_COLOR, int, int, int, const char *, ...);
/* sound */
WINPIXELDLL int WINPIXELCALL wpx_sound_play (const char *, int);
WINPIXELDLL int WINPIXELCALL wpx_music_open (const char *, const char *);
WINPIXELDLL int WINPIXELCALL wpx_music_play (const char *);
WINPIXELDLL int WINPIXELCALL wpx_music_pause (const char *);
WINPIXELDLL int WINPIXELCALL wpx_music_resume (const char *);
WINPIXELDLL int WINPIXELCALL wpx_music_volume (const char *, int);
WINPIXELDLL int WINPIXELCALL wpx_music_balance (const char *, int);
WINPIXELDLL long WINPIXELCALL wpx_music_length (const char *);
WINPIXELDLL long WINPIXELCALL wpx_music_position (const char *);
WINPIXELDLL int WINPIXELCALL wpx_music_seek (const char *, long);
WINPIXELDLL int WINPIXELCALL wpx_music_close (const char *);
/* sprite */
WINPIXELDLL void WINPIXELCALL wpx_draw_image (uint32_t *, rec4i, rec4i);
WINPIXELDLL uint32_t WINPIXELCALL *wpx_load_image (const char *, int *, int *);

#ifdef __cplusplus
} // __cplusplus defined.
#endif

#endif

/* --TEMPLATE--

gcc -o main main.c -Wall -IC:\WINPIXEL -L. -lwinpixel

#include <winpixel.h>

int main (void) {
	
	winpixel_window("template", 800, 600);

	// main loop
	while (wpx_exit && !key(VK_ESCAPE)) {

		wpx_line(WPX_W2, WPX_H2, wpx_mouse.x, wpx_mouse.y, WHITE);

		winpixel_render(0x19294E, 2);
	}

	winpixel_close();

	return 0;
}
*/