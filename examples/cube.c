#include "winpixel.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define SCR_W 256
#define SCR_H 144
#undef  NEAR
#undef  FAR
#define NEAR  0.1f
#define FAR   20.0f

/* camera at (0,0,-CAM_Z) looking +Z */
#define CAM_Z  3.5f
/* FOCAL = (SCR_H/2) / tan(30°)  →  60° vertical FOV */
#define FOCAL  233.8f

/* ── 17 dither patterns (from s-buffer.lua) ───────────────────
   pixel (x,y) is on if: (pat >> (15 - (x&3) - (y&3)*4)) & 1  */
static const uint16_t b_tbl[17] = {
    0x0000, 0x0001, 0x8020, 0x080a, 0x050a,
    0x5250, 0x8525, 0x25a5, 0xa5a5, 0xa5ad,
    0xada7, 0xfada, 0xfaf5, 0x5f7f, 0xdf7f,
    0xfeff, 0xffff
};

/* ── palette per face (dark, light) ────────────────────────────── */
static const Color32 fpal[6][2] = {
    { 0x000040FF, 0x29ADFFFF },  /* +Z  blue     */
    { 0x003B00FF, 0x00E436FF },  /* -Z  green    */
    { 0x3F2000FF, 0xFF8C00FF },  /* -X  orange   */
    { 0x3F0010FF, 0xFF004DFF },  /* +X  red      */
    { 0x303030FF, 0xC0C0C0FF },  /* +Y  gray     */
    { 0x3F3F00FF, 0xFFEC27FF },  /* -Y  yellow   */
};

/* ── cube geometry ─────────────────────────────────────────────── */
static const float cv[8][3] = {
    {-1,-1,-1},{1,-1,-1},{1,1,-1},{-1,1,-1},
    {-1,-1, 1},{1,-1, 1},{1,1, 1},{-1,1, 1}
};
static const int cf[6][4] = {
    {4,5,6,7}, {1,0,3,2},   /* +Z, -Z */
    {0,4,7,3}, {5,1,2,6},   /* -X, +X */
    {7,6,2,3}, {0,1,5,4},   /* +Y, -Y */
};
static const float cn[6][3] = {
    {0,0,1},{0,0,-1},{-1,0,0},{1,0,0},{0,1,0},{0,-1,0}
};

/* light from (1,2,-1), normalized */
static const float LDIR[3] = { 0.4082f, 0.8165f, -0.4082f };

/* ── math 3×3 row-major ──────────────────────────────────────── */
static void m3_identity(float m[9]) {
    memset(m, 0, 36); m[0] = m[4] = m[8] = 1.0f;
}
static void m3_mul(const float a[9], const float b[9], float o[9]) {
    for (int r=0; r<3; r++)
        for (int c=0; c<3; c++)
            o[r*3+c] = a[r*3]*b[c]+a[r*3+1]*b[3+c]+a[r*3+2]*b[6+c];
}
static void m3_mulv(const float m[9], const float v[3], float o[3]) {
    o[0]=m[0]*v[0]+m[1]*v[1]+m[2]*v[2];
    o[1]=m[3]*v[0]+m[4]*v[1]+m[5]*v[2];
    o[2]=m[6]*v[0]+m[7]*v[1]+m[8]*v[2];
}
static void m3_roty(float m[9], float a) {
    float c=cosf(a), s=sinf(a);
    m3_identity(m); m[0]=c; m[2]=s; m[6]=-s; m[8]=c;
}
static void m3_rotx(float m[9], float a) {
    float c=cosf(a), s=sinf(a);
    m3_identity(m); m[4]=c; m[5]=-s; m[7]=s; m[8]=c;
}

/* ── projection ───────────────────────────────────────────────── */
static void proj_v(const float v[3], float *sx, float *sy, float *sz) {
    float z = v[2] + CAM_Z;
    if (z < 0.01f) z = 0.01f;
    float w = FOCAL / z;
    *sx = SCR_W * 0.5f + v[0] * w;
    *sy = SCR_H * 0.5f - v[1] * w;
    *sz = z;
}

/* ── flat-dither rasterizer ─────────────────────────────────────── */
static void hline(int y, int x0, int x1,
                  uint16_t pat, Color32 dk, Color32 lt) {
    if (x0 > x1) { int t=x0; x0=x1; x1=t; }
    if (y<0||y>=SCR_H||x1<0||x0>=SCR_W) return;
    if (x0<0) x0=0;
    if (x1>=SCR_W) x1=SCR_W-1;
    int row = (y&3)<<2;
    for (int x=x0; x<=x1; x++)
        wpx_pixel(x, y, (pat >> (15-(x&3)-row)) & 1 ? lt : dk);
}

typedef struct { float x, y; } V2;

/* sub-pixel stepping to avoid wobble on the edges */
static void fill_tri(V2 a, V2 b, V2 c,
                     uint16_t pat, Color32 dk, Color32 lt) {
    if (a.y>b.y){V2 t=a;a=b;b=t;}
    if (a.y>c.y){V2 t=a;a=c;c=t;}
    if (b.y>c.y){V2 t=b;b=c;c=t;}

    float h13 = c.y - a.y;
    if (h13 < 0.5f) return;
    float dxac = (c.x - a.x) / h13;

    float h12 = b.y - a.y;
    if (h12 > 0.5f) {
        float dxab = (b.x - a.x) / h12;
        int   y0 = (int)ceilf(a.y);
        float ps = y0 - a.y;
        float xac = a.x + ps*dxac, xab = a.x + ps*dxab;
        for (int y=y0; y<(int)ceilf(b.y); y++) {
            hline(y, (int)xac, (int)xab, pat, dk, lt);
            xac += dxac; xab += dxab;
        }
    }
    float h23 = c.y - b.y;
    if (h23 > 0.5f) {
        float dxbc = (c.x - b.x) / h23;
        int   y0 = (int)ceilf(b.y);
        float xac = a.x + (y0-a.y)*dxac;
        float xbc = b.x + (y0-b.y)*dxbc;
        for (int y=y0; y<(int)ceilf(c.y); y++) {
            hline(y, (int)xac, (int)xbc, pat, dk, lt);
            xac += dxac; xbc += dxbc;
        }
    }
}

/* ── sort back-to-front ──────────────────────────────────────── */
typedef struct { int face; float depth; } FE;
static int cmp_fe(const void *a, const void *b) {
    float da=((FE*)a)->depth, db=((FE*)b)->depth;
    return da<db ? 1 : da>db ? -1 : 0;
}

/* ── main ────────────────────────────────────────────────────── */
int main(void) {
    winpixel_window("Cube Dither", 800, 600, false);
    wpx_screen_resolution(SCR_W, SCR_H, true);

    float ax = 0.3f, ay = 0.0f;
    float scale = 0.5f;
    float mx[9], my[9], rot[9];
    float vw[8][3];
    float vsx[8], vsy[8], vsz[8];

    while (wpx_quit && !wpx_key_down(VK_ESCAPE)) {
        float dt = (float)wpx_get_delta();
        ay += 0.40f * dt;
        ax += 0.15f * dt;

        if (wpx_mouse_wheel > 0) scale = clamp(scale * 1.12f, 0.1f, 8.0f);
        if (wpx_mouse_wheel < 0) scale = clamp(scale * 0.89f, 0.1f, 8.0f);

        m3_roty(my, ay);
        m3_rotx(mx, ax);
        m3_mul(mx, my, rot);

        for (int i=0; i<8; i++) {
            float sv[3] = { cv[i][0]*scale, cv[i][1]*scale, cv[i][2]*scale };
            m3_mulv(rot, sv, vw[i]);
            proj_v(vw[i], &vsx[i], &vsy[i], &vsz[i]);
        }

        /* backface cull + sort */
        FE faces[6]; int nf = 0;
        for (int f=0; f<6; f++) {
            float wn[3];
            m3_mulv(rot, cn[f], wn);
            if (wn[2] >= 0.0f) continue;  /* points away from the camera */
            float d = vsz[cf[f][0]]+vsz[cf[f][1]]+
                      vsz[cf[f][2]]+vsz[cf[f][3]];
            faces[nf++] = (FE){f, d};
        }
        qsort(faces, nf, sizeof(FE), cmp_fe);

        /* rasterize */
        for (int k=0; k<nf; k++) {
            int f = faces[k].face;
            float wn[3];
            m3_mulv(rot, cn[f], wn);

            /* flat per-face brightness with a small ambient floor */
            float br = 0.1f + 0.9f * fmaxf(0.0f,
                wn[0]*LDIR[0] + wn[1]*LDIR[1] + wn[2]*LDIR[2]);
            int bi = (int)(br * 16.0f + 0.5f);
            if (bi > 16) bi = 16;

            uint16_t pat = b_tbl[bi];
            Color32  dk  = fpal[f][0], lt = fpal[f][1];

            /* quad → 2 triangles */
            V2 p[4];
            for (int i=0; i<4; i++)
                p[i] = (V2){ vsx[cf[f][i]], vsy[cf[f][i]] };
            fill_tri(p[0], p[1], p[2], pat, dk, lt);
            fill_tri(p[0], p[2], p[3], pat, dk, lt);
        }

        wpx_text(WHITE, 0, 4, 4, "FPS: %.0f  scale: %.2f", wpx_get_fps(), scale);
        winpixel_render(BLUEPRINT, 1);
    }
    return 0;
}
