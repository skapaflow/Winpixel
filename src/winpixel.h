/*
 * winpixel.h — Winpixel public API
 *
 * 2D software-rendered graphics library for Windows.
 *
 * Color format:      Color32 = 0xRRGGBBAA  (e.g. RED = 0xFF0000FF)
 * Coordinate system: origin (0,0) at top-left, Y increases downward.
 *
 * Typical usage:
 *
 *   winpixel_window("Title", 800, 600, true);
 *   while (wpx_quit && !wpx_key_down(VK_ESCAPE)) {
 *       // draw here
 *       winpixel_render(BLACK, 1);
 *   }
 */

#ifndef WINPIXEL_H
#define WINPIXEL_H

#ifdef WINPIXEL_DLL
    #define WINPIXELDLL __declspec(dllexport)
#else
    #define WINPIXELDLL __declspec(dllimport)
#endif

#define WINPIXELCALL __cdecl

#ifdef __cplusplus
extern "C" {
#endif

#include <windows.h>
#include <stdbool.h>
#include <stdint.h>

#define WPXMIN 128
#define WPXMID 256
#define WPXMAX 1024
#define WPXHIG 5000

#define BLANK      0x00000000
#define LIGHTRED   0xff6060FF
#define RED        0xFF0000FF
#define DARKRED    0x800000FF
#define PINK       0xFF80FFFF
#define MAGENTA    0xFF00FFFF
#define PURPLE     0x400080FF
#define SKYBLUE    0x0080FFFF
#define BLUE       0x0000FFFF
#define DARKBLUE   0x000080FF
#define CYAN       0x00FFFFFF
#define DARKCYAN   0x008080FF
#define PETROBLUE  0x084d6eFF
#define BLUEPRINT  0x19294EFF
#define LIME       0x00FF00FF
#define GREEN      0x008000FF
#define DARKGREEN  0x004000FF
#define YELLOW     0xFFFF00FF
#define DARKYELLOW 0x808000FF
#define ORANGE     0xff8000FF
#define BRAWN      0x804000FF
#define BLACK      0x000000FF
#define WHITE      0xFFFFFFFF
#define GRAY       0xC0C0C0FF
#define DARKGRAY   0x808080FF
/* NOTE: no issues with negative numbers, but overflows for very large positive values */
#define WPXSET(d,x,y) (int) (abs(x) + (abs(y) * d.w))
#define WPXSET2(d,x,y) (int) (abs(x) + (abs(y) * (d)))
#define WPXSWAP(t, x, y) do {t __wpxswap__ = x; x = y; y = __wpxswap__;} while (0)
/* * */
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
/* * */
#ifndef M_PI
	#define M_PI 3.14159265358979323846
#endif
#define M_TAU 6.28318530717958647692
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
// Generates a random uint32_t safely regardless of RAND_MAX
#define hrandom() ( ((uint32_t)(rand() & 0xFF) << 24) | \
                    ((uint32_t)(rand() & 0xFF) << 16) | \
                    ((uint32_t)(rand() & 0xFF) <<  8) | \
                    ((uint32_t)(rand() & 0xFF)) )
// Generates a random opaque color (alpha always 0xFF)
#define hrandom_opaque() ( ((uint32_t)(rand() & 0xFF) << 24) | \
                           ((uint32_t)(rand() & 0xFF) << 16) | \
                           ((uint32_t)(rand() & 0xFF) <<  8) | \
                           0x000000FF )
/* arandom(1,2,3) — picks a random element from the given list */
static inline int wpx_rand_array_int(int arr[], int size) {return arr[rand() % size];}
#define arandom(...)\
    wpx_rand_array_int(\
    (int[]) {__VA_ARGS__},\
    sizeof((int[]) {__VA_ARGS__})\
    / sizeof((int[]) {__VA_ARGS__}[0]))
/* * */
static inline float wpx_rand_array_float(float arr[], int size) {return arr[rand() % size];}
#define afrandom(...)\
    wpx_rand_array_float(\
    (float[]) {__VA_ARGS__},\
    sizeof((float[]) {__VA_ARGS__})\
    / sizeof((float[]) {__VA_ARGS__}[0]))
/* ---------------------------------------------------------------
 * vector 2
 * --------------------------------------------------------------- */
typedef struct { int    x, y; } vec2i;
typedef struct { float  x, y; } vec2f;
typedef struct { double x, y; } vec2d;
#define VEC2I ((vec2i) {    0,    0 })
#define VEC2F ((vec2f) { 0.0f, 0.0f })
#define VEC2D ((vec2d) {  0.0,  0.0 })
#define vec2i(x, y) ((vec2i) {         (x),         (y) })
#define vec2f(x, y) ((vec2f) {  (float)(x),  (float)(y) })
#define vec2d(x, y) ((vec2d) { (double)(x), (double)(y) })
/* ---------------------------------------------------------------
 * vector 3
 * --------------------------------------------------------------- */
typedef struct { int    x, y, z; } vec3i;
typedef struct { float  x, y, z; } vec3f;
typedef struct { double x, y, z; } vec3d;
#define VEC3I ((vec3i) {    0,    0,    0 })
#define VEC3F ((vec3f) { 0.0f, 0.0f, 0.0f })
#define VEC3D ((vec3d) {  0.0,  0.0,  0.0 })
#define vec3i(x, y, z) ((vec3i) {         (x),         (y),         (z) })
#define vec3f(x, y, z) ((vec3f) {  (float)(x),  (float)(y),  (float)(z) })
#define vec3d(x, y, z) ((vec3d) { (double)(x), (double)(y), (double)(z) })
/* ---------------------------------------------------------------
 * vector 4
 * --------------------------------------------------------------- */
typedef struct { int    x, y, z, w; } vec4i;
typedef struct { float  x, y, z, w; } vec4f;
typedef struct { double x, y, z, w; } vec4d;
#define VEC4I ((vec4i) {    0,    0,    0,    0 })
#define VEC4F ((vec4f) { 0.0f, 0.0f, 0.0f, 0.0f })
#define VEC4D ((vec4d) {  0.0,  0.0,  0.0,  0.0 })
#define vec4i(x, y, z, w) ((vec4i) {         (x),         (y),         (z),         (w) })
#define vec4f(x, y, z, w) ((vec4f) {  (float)(x),  (float)(y),  (float)(z),  (float)(w) })
#define vec4d(x, y, z, w) ((vec4d) { (double)(x), (double)(y), (double)(z), (double)(w) })
/* ---------------------------------------------------------------
 * rect (x, y, w, h)
 * --------------------------------------------------------------- */
typedef struct { int    x, y, w, h; } recti;
typedef struct { float  x, y, w, h; } rectf;
typedef struct { double x, y, w, h; } rectd;
#define RECTI ((recti) {    0,    0,    0,    0 })
#define RECTF ((rectf) { 0.0f, 0.0f, 0.0f, 0.0f })
#define RECTD ((rectd) {  0.0,  0.0,  0.0,  0.0 })
#define recti(x, y, w, h) ((recti) {         (x),         (y),         (w),         (h) })
#define rectf(x, y, w, h) ((rectf) {  (float)(x),  (float)(y),  (float)(w),  (float)(h) })
#define rectd(x, y, w, h) ((rectd) { (double)(x), (double)(y), (double)(w), (double)(h) })
/* ---------------------------------------------------------------
 * Vario (in, out)
 * --------------------------------------------------------------- */
typedef struct { int    in, out; } varioi;
typedef struct { float  in, out; } variof;
typedef struct { double in, out; } variod;
#define VARIOI ((varioi) {    0,    0 })
#define VARIOF ((variof) { 0.0f, 0.0f })
#define VARIOD ((variod) {  0.0,  0.0 })
#define varioi(i, o) ((varioi) {         (i),         (o) })
#define variof(i, o) ((variof) {  (float)(i),  (float)(o) })
#define variod(i, o) ((variod) { (double)(i), (double)(o) })
/* ---------------------------------------------------------------
 * Color RGBA Format!
 * --------------------------------------------------------------- */
typedef uint32_t Color32;
typedef struct {float   r, g, b, a;} Colorf;
typedef struct {uint8_t r, g, b, a;} Color8;
typedef struct {Colorf     in, out;} Coloriof;
#define Colorf(r, g, b, a) ((Colorf) {   (float)(r),   (float)(g),   (float)(b),    (float)(a) })
#define Color8(r, g, b, a) ((Color8) { (uint8_t)(r), (uint8_t)(g), (uint8_t)(b),  (uint8_t)(a) })
#define Coloriof(in,out)   ((Coloriof){ (in), (out) })

struct WINPIXEL;
typedef struct WINPIXEL WINPIXEL;

/* ---------------------------------------------------------------
 * Sprite
 * --------------------------------------------------------------- */
typedef struct {
    uint32_t *pixels;
    int       w, h;
} WPX_Sprite;

extern WINPIXELDLL bool wpx_quit;
extern WINPIXELDLL int  WPX_W, WPX_H;
extern WINPIXELDLL int  WPX_W2, WPX_H2;
// -------------------------
// Window
// -------------------------
WINPIXELDLL int  WINPIXELCALL winpixel_window (const char *title, int w, int h, bool logo); /* logo=true prints ASCII banner */
WINPIXELDLL void WINPIXELCALL winpixel_maximize (void);
WINPIXELDLL bool WINPIXELCALL winpixel_poll_events (void);
WINPIXELDLL void WINPIXELCALL winpixel_render (Color32 background, int delay); /* present + clear + sleep(delay ms) */
WINPIXELDLL void WINPIXELCALL winpixel_present (void);                         /* blit framebuffer, update time/input */
WINPIXELDLL HWND WINPIXELCALL winpixel_get_hwnd (void);
WINPIXELDLL HDC  WINPIXELCALL winpixel_get_hdc (void);                         /* caller must ReleaseDC when done */
// -------------------------
// Input - constants
// -------------------------
#define WPX_MAX_KEYS          256
#define WPX_MAX_MOUSE_BUTTONS 3
// -------------------------
// Input - state (read-only)
// -------------------------
extern WINPIXELDLL vec2f  wpx_mouse;
extern WINPIXELDLL int8_t wpx_mouse_wheel;         /* return (-1,0,1) */
extern WINPIXELDLL bool   wpx_mouse_wheel_down;
extern WINPIXELDLL bool   wpx_mouse_wheel_press;
extern WINPIXELDLL bool   wpx_mouse_left_down;
extern WINPIXELDLL bool   wpx_mouse_left_press;
extern WINPIXELDLL bool   wpx_mouse_left_up;
extern WINPIXELDLL bool   wpx_mouse_right_down;
extern WINPIXELDLL bool   wpx_mouse_right_press;
extern WINPIXELDLL bool   wpx_mouse_right_up;
// -------------------------
// Input - public API
// -------------------------
WINPIXELDLL bool WINPIXELCALL wpx_key_down  (int key);
WINPIXELDLL bool WINPIXELCALL wpx_key_press (int key);
WINPIXELDLL bool WINPIXELCALL wpx_key_up    (int key);
WINPIXELDLL bool WINPIXELCALL wpx_mouse_button_down (int button);
// -------------------------
// Time - public API
// -------------------------
WINPIXELDLL double WINPIXELCALL wpx_get_fps(void);
WINPIXELDLL double WINPIXELCALL wpx_get_delta(void);
// -------------------------
// Draw - public API
// -------------------------
WINPIXELDLL Color32 WINPIXELCALL rgba_to_hex (int r, int g, int b, int a);
WINPIXELDLL Colorf WINPIXELCALL hex_to_Colorf (Color32 h);
WINPIXELDLL Color32 WINPIXELCALL Colorf_to_hex (Colorf color);
WINPIXELDLL Colorf WINPIXELCALL lerp_color (Colorf start, Colorf end, float time);
WINPIXELDLL void WINPIXELCALL winpixel_clear  (Color32 color);
WINPIXELDLL void WINPIXELCALL wpx_pixel (int x, int y, Color32 color);
WINPIXELDLL Color32 WINPIXELCALL wpx_getpixel (int x, int y);
WINPIXELDLL void WINPIXELCALL wpx_line (int x0, int y0, int x1, int y1, Color32 color);
WINPIXELDLL void WINPIXELCALL wpx_line_gap (int x, int y, int w, int h, Color32 color);            /* draws middle 50% of the segment */
WINPIXELDLL void WINPIXELCALL wpx_line_dash (int x, int y, int w, int h, float dash, Color32 color); /* single shortened line with inset by dash */
WINPIXELDLL void WINPIXELCALL wpx_line_dashed (int x, int y, int w, int h, float dash, Color32 color); /* segmented dashed line */
WINPIXELDLL void WINPIXELCALL wpx_rect (int x, int y, int w, int h, Color32 color);
WINPIXELDLL void WINPIXELCALL wpx_rect_center (int x, int y, int w, int h, Color32 color);         /* x,y is the center */
WINPIXELDLL void WINPIXELCALL wpx_rect_fill (int x, int y, int w, int h, Color32 color);
WINPIXELDLL void WINPIXELCALL wpx_rect_fill_grid (int x, int y, int w, int h, Color32 color);      /* 50% screen-door transparency */
WINPIXELDLL void WINPIXELCALL wpx_circle (int x, int y, int radius, Color32 color);
WINPIXELDLL void WINPIXELCALL wpx_circle_fill (int xc, int yc, int radius, Color32 color);
WINPIXELDLL void WINPIXELCALL wpx_circle_fill_grid (int xc, int yc, int radius, Color32 color);    /* 50% screen-door transparency */
WINPIXELDLL void WINPIXELCALL wpx_triangle_fill (vec2i *triangle, Color32 color);                   /* triangle[3] */
WINPIXELDLL void WINPIXELCALL wpx_triangle_fill_grid (vec2i *triangle, Color32 color);              /* triangle[3], 50% screen-door transparency */
WINPIXELDLL void WINPIXELCALL wpx_triangle_fill_ex (vec2f *triangle, float x, float y, float scale, float angle, Color32 color); /* triangle[3], rotate+scale+translate */
WINPIXELDLL void WINPIXELCALL wpx_ellipse (int x, int y, int radius_x, int radius_y, Color32 color);
WINPIXELDLL void WINPIXELCALL wpx_bezier_thick (vec2f startPos, vec2f endPos, float thick, Color32 color); /* cubic bezier, no control points */
WINPIXELDLL void WINPIXELCALL wpx_spline (float px, float py, float pw, float ph, float thick, Color32 color); /* thick line segment */
WINPIXELDLL void WINPIXELCALL wpx_spline_gap (int x, int y, int w, int h, float thick, Color32 color);
WINPIXELDLL void WINPIXELCALL wpx_spline_dashed (int x, int y, int w, int h, float thick, float dash, Color32 color);
WINPIXELDLL void WINPIXELCALL wpx_spline_bezier_quadratic (const vec2f *points, int pointCount, float thick, Color32 color); /* [p1,c2,p3,c4,...] min 3 pts */
WINPIXELDLL void WINPIXELCALL wpx_spline_bezier_cubic (const vec2f *points, int pointCount, float thick, Color32 color);    /* [p1,c2,c3,p4,...] min 4 pts */
// -------------------------
// Print - public API
// -------------------------
WINPIXELDLL void WINPIXELCALL wpx_text (Color32 color, int scale, int x, int y, const char *fmt, ...);
WINPIXELDLL void WINPIXELCALL wpx_text_center (Color32 color, int scale, int x, int y, const char *fmt, ...);
WINPIXELDLL void WINPIXELCALL wpx_text_ex (int center, Color32 color, int scale, Color32 bcolor, int gap, int x, int y, const char *fmt, ...); /* center=1 centers text; bcolor=background shadow color */
WINPIXELDLL void WINPIXELCALL wpx_text_measure (const char *str, int scale, int *w, int *h); /* total pixel size of str at given scale */
WINPIXELDLL const char WINPIXELCALL *cat (const char *fmt, ...);
// -------------------------
// Sprite - public API
// -------------------------
WINPIXELDLL WPX_Sprite *WINPIXELCALL wpx_sprite_create (int w, int h); /* RGBA Format! */
WINPIXELDLL WPX_Sprite *WINPIXELCALL wpx_sprite_load (const char *path); /* RGBA Format! */
WINPIXELDLL void WINPIXELCALL wpx_sprite_free (WPX_Sprite *s);
WINPIXELDLL void WINPIXELCALL wpx_sprite_set_pixel (WPX_Sprite *s, int x, int y, Color32 color);
WINPIXELDLL Color32 WINPIXELCALL wpx_sprite_get_pixel (const WPX_Sprite *s, int x, int y);
WINPIXELDLL void WINPIXELCALL wpx_sprite_fill (WPX_Sprite *s, Color32 color);
WINPIXELDLL void WINPIXELCALL wpx_sprite_clear (WPX_Sprite *s);
WINPIXELDLL void WINPIXELCALL wpx_sprite_draw (const WPX_Sprite *s, int x, int y);
WINPIXELDLL void WINPIXELCALL wpx_sprite_draw_scale (const WPX_Sprite *s, int x, int y, int scale);
WINPIXELDLL void WINPIXELCALL wpx_sprite_draw_flip (const WPX_Sprite *s, int x, int y, bool flip_x, bool flip_y);
WINPIXELDLL void WINPIXELCALL wpx_sprite_draw_sub (const WPX_Sprite *s, int x, int y, recti src);
WINPIXELDLL void WINPIXELCALL wpx_write_png (const char *name); /* save framebuffer to PNG */
// -------------------------
// Math - public API
// -------------------------
typedef struct {
	int   oldstep;
	int   param_1;
	int   param_2;
	float currentX;
	float increment;
} Noisegen;

WINPIXELDLL int WINPIXELCALL fequal (float a, float b);
WINPIXELDLL char WINPIXELCALL *strfmt (char *buf, int64_t value);          /* buf must be >= 17 bytes; formats with dot separators */
WINPIXELDLL const char WINPIXELCALL *vbytes (int64_t v);                   /* returns unit string: " B", "KB", "MB", "GB", "TB" */
WINPIXELDLL float WINPIXELCALL lerp (float start, float end, float time);
WINPIXELDLL float WINPIXELCALL invlerp (float start, float end, float time);
WINPIXELDLL float WINPIXELCALL normalize (float x, float min, float max);
WINPIXELDLL float WINPIXELCALL map (float value, float inputMin, float inputMax, float outputMin, float outputMax);
WINPIXELDLL float WINPIXELCALL wrap (float value, float min, float max);   /* loops value within [min, max] */
WINPIXELDLL float WINPIXELCALL clamp (float x, float lower, float upper);
WINPIXELDLL float WINPIXELCALL distance_point (float x, float y, float x2, float y2);
WINPIXELDLL float WINPIXELCALL direction_point (float x, float y, float x2, float y2); /* returns angle in degrees [0, 360) */
WINPIXELDLL int WINPIXELCALL isprime (int number);
WINPIXELDLL uint32_t WINPIXELCALL hash (const char *str);                  /* Jenkins one-at-a-time hash */
WINPIXELDLL void WINPIXELCALL int_shuffle (int *array, int size);
WINPIXELDLL int WINPIXELCALL line_in_point (rectf line, vec2f point, float tolerance);
WINPIXELDLL int WINPIXELCALL line_in_line (rectf line1, rectf line2);
WINPIXELDLL vec2f WINPIXELCALL middle_line (float x, float y, float x2, float y2);
WINPIXELDLL int32_t WINPIXELCALL sqrt_int (int32_t value);                 /* integer square root, no FPU */
WINPIXELDLL void WINPIXELCALL noise_generator (Noisegen *gen, float increment); /* init; call before noise() */
WINPIXELDLL float WINPIXELCALL noise (Noisegen *gen);                      /* 1D Perlin noise, output in [-0.5, +0.5] */
WINPIXELDLL void WINPIXELCALL noise_reset (Noisegen *gen);                 /* resets position to 0 */
WINPIXELDLL void WINPIXELCALL noise_increment (Noisegen *gen, float newIncrement);

#ifdef __cplusplus
}
#endif

#endif // WINPIXEL_H