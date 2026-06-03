#include <winpixel.h>
#include <string.h>

#define SCR_W 800
#define SCR_H 400

int main(void) {
    winpixel_window("Perlin Noise Terrain", SCR_W, SCR_H, false);

    Noisegen gen;
    noise_generator(&gen, 0.018f);

    float heights[SCR_W];
    for (int x = 0; x < SCR_W; x++)
        heights[x] = noise(&gen);

    float speed  = 40.0f;   /* pixels per second */
    float scroll = 0.0f;
    bool  paused = false;

    Colorf c_deep  = hex_to_Colorf(DARKBLUE);
    Colorf c_water = hex_to_Colorf(PETROBLUE);
    Colorf c_sand  = hex_to_Colorf(DARKYELLOW);
    Colorf c_grass = hex_to_Colorf(DARKGREEN);
    Colorf c_rock  = hex_to_Colorf(GRAY);
    Colorf c_snow  = hex_to_Colorf(WHITE);

    while (wpx_quit && !wpx_key_down(VK_ESCAPE)) {
        float dt = (float)wpx_get_delta();

        if (wpx_key_press(VK_SPACE)) paused = !paused;
        if (wpx_key_press('R')) {
            noise_reset(&gen);
            for (int x = 0; x < SCR_W; x++) heights[x] = noise(&gen);
        }
        if (wpx_key_down(VK_UP))   speed = clamp(speed + 30.0f * dt, 2.0f, 300.0f);
        if (wpx_key_down(VK_DOWN)) speed = clamp(speed - 30.0f * dt, 2.0f, 300.0f);

        if (!paused) {
            scroll += speed * dt;
            while (scroll >= 1.0f) {
                memmove(heights, heights + 1, (SCR_W - 1) * sizeof(float));
                heights[SCR_W - 1] = noise(&gen);
                scroll -= 1.0f;
            }
        }

        for (int x = 0; x < SCR_W; x++) {
            float h = heights[x];   /* [-0.5, +0.5] */
            float t = map(h, -0.5f, 0.5f, 0.0f, 1.0f);
            int   y = (int)map(h, -0.5f, 0.5f, SCR_H * 0.08f, SCR_H * 0.92f);

            Colorf col;
            if      (t < 0.25f) col = lerp_color(c_deep,  c_water, t / 0.25f);
            else if (t < 0.38f) col = lerp_color(c_water, c_sand,  (t - 0.25f) / 0.13f);
            else if (t < 0.65f) col = lerp_color(c_sand,  c_grass, (t - 0.38f) / 0.27f);
            else if (t < 0.82f) col = lerp_color(c_grass, c_rock,  (t - 0.65f) / 0.17f);
            else                col = lerp_color(c_rock,  c_snow,  (t - 0.82f) / 0.18f);

            wpx_line(x, y, x, SCR_H, Colorf_to_hex(col));
        }

        wpx_text(WHITE, 0, 4, 4,          "FPS %.0f  |  SPEED %.0f px/s",
                 wpx_get_fps(), speed);
        wpx_text(DARKGRAY, 0, 4, SCR_H - 12,
                 "UP/DOWN=speed  SPACE=pause  R=reset  ESC=quit");
        if (paused)
            wpx_text_center(YELLOW, 1, SCR_W / 2, 4, "PAUSED");

        winpixel_render(DARKBLUE, 1);
    }
    return 0;
}
