#include <winpixel.h>
#include <math.h>

#define SCR_W 800
#define SCR_H 500
#define GW  240   /* graph width  */
#define GH  140   /* graph height */
#define GY  300   /* graph top-y  */
#define GAP  12   /* gap between graphs */

int main (void) {

    winpixel_window("TimeGraph Demo", SCR_W, SCR_H, false);
    wpx_screen_resolution(SCR_W, SCR_H, false);

    WPX_TimeGraph tg_fps   = {0};
    WPX_TimeGraph tg_sin   = {0};
    WPX_TimeGraph tg_noise = {0};

    Noisegen gen;
    noise_generator(&gen, 0.04f);

    float time  = 0.0f;
    float peak  = 120.0f;   /* FPS graph normalizer (auto-scales) */

    /* layout: 3 graphs centered */
    int total_w = 3 * GW + 2 * GAP;
    int gx0     = (SCR_W - total_w) / 2;

    while (wpx_quit && !wpx_key_down(VK_ESCAPE)) {
        float dt  = (float)wpx_get_delta();
        float fps = (float)wpx_get_fps();
        time += dt;

        /* auto-scale FPS peak */
        if (fps > peak) peak = fps * 1.05f;
        else            peak = peak * 0.9998f + fps * 0.0002f;

        /* signals */
        float v_fps   = fps / peak;
        float v_sin   = (sinf(time * 2.0f) + 1.0f) * 0.5f;
        float v_noise = clamp(noise(&gen) + 0.5f, 0.0f, 1.0f);

        /* ---- graphs ---- */
        int gx;

        gx = gx0;
        wpx_timegraph(&tg_fps,   v_fps,   gx, GY, GW, GH, LIME);
        wpx_text(LIME,  0, gx + 4, GY + GH + 4, "fps  %.0f / %.0f", fps, peak);
        wpx_text_center(GRAY, 0, gx + GW/2, GY - 14, "FPS");

        gx = gx0 + GW + GAP;
        wpx_timegraph(&tg_sin,   v_sin,   gx, GY, GW, GH, SKYBLUE);
        wpx_text(SKYBLUE, 0, gx + 4, GY + GH + 4, "sin  %.3f", v_sin);
        wpx_text_center(GRAY, 0, gx + GW/2, GY - 14, "sin(t)");

        gx = gx0 + 2 * (GW + GAP);
        wpx_timegraph(&tg_noise, v_noise, gx, GY, GW, GH, ORANGE);
        wpx_text(ORANGE, 0, gx + 4, GY + GH + 4, "noise %.3f", v_noise);
        wpx_text_center(GRAY, 0, gx + GW/2, GY - 14, "Perlin noise");

        /* ---- top illustration: signals drawn as live waveforms ---- */
        static float history_sin  [SCR_W] = {0};
        static float history_noise[SCR_W] = {0};

        memmove(history_sin,   history_sin   + 1, (SCR_W - 1) * sizeof(float));
        memmove(history_noise, history_noise + 1, (SCR_W - 1) * sizeof(float));
        history_sin  [SCR_W - 1] = v_sin;
        history_noise[SCR_W - 1] = v_noise;

        int wave_y = 160, wave_h = 120;
        wpx_rect_fill(0, wave_y - wave_h/2 - 4, WPX_W, wave_h + 8, 0x0D1117FF);
        wpx_line(0, wave_y, WPX_W, wave_y, 0x1E2836FF);   /* center line */

        for (int x = 1; x < SCR_W; x++) {
            int ys0 = wave_y - (int)((history_sin  [x-1] - 0.5f) * wave_h);
            int ys1 = wave_y - (int)((history_sin  [x  ] - 0.5f) * wave_h);
            int yn0 = wave_y - (int)((history_noise[x-1] - 0.5f) * wave_h);
            int yn1 = wave_y - (int)((history_noise[x  ] - 0.5f) * wave_h);
            wpx_line(x - 1, ys0, x, ys1, SKYBLUE);
            wpx_line(x - 1, yn0, x, yn1, ORANGE);
        }

        /* labels */
        wpx_text(SKYBLUE, 0, 4, wave_y - wave_h/2 - 2, "sin");
        wpx_text(ORANGE,  0, 4, wave_y - wave_h/2 + 8,  "noise");

        /* title */
        wpx_text(WHITE, 1, WPX_W2 - 80, 10, "wpx_timegraph  demo");
        wpx_text(DARKGRAY, 0, 4, WPX_H - 12, "ESC = quit");

        winpixel_render(0x0A0A12FF, 1);
    }
    return 0;
}
