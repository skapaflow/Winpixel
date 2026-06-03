#include <winpixel.h>

#define BG 0x1A1A1AFF

int main(void) {
    winpixel_window("Paint", 640, 480, false);

    Color32 palette[] = {
        WHITE, LIGHTRED, ORANGE, YELLOW, GREEN,
        CYAN, BLUE, PURPLE, PINK, DARKGRAY
    };
    int     npal    = 10;
    int     sel     = 0;
    Color32 col     = WHITE;
    int     brush   = 8;
    int     pal_x   = WPX_W - 32;
    int     hud_h   = 28;

    /* initial canvas */
    wpx_rect_fill(0, hud_h, pal_x, WPX_H - hud_h, BG);

    while (wpx_quit && !wpx_key_down(VK_ESCAPE)) {

        /* brush size */
        if      (wpx_mouse_wheel >  0) brush = (int)clamp((float)brush + 2.0f, 1.0f, 60.0f);
        else if (wpx_mouse_wheel < 0) brush = (int)clamp((float)brush - 2.0f, 1.0f, 60.0f);

        /* palette click */
        if (wpx_mouse_left_press && (int)wpx_mouse.x >= pal_x) {
            for (int i = 0; i < npal; i++) {
                int py = hud_h + 8 + i * 26;
                if ((int)wpx_mouse.y >= py && (int)wpx_mouse.y < py + 20) {
                    sel = i;
                    col = palette[i];
                }
            }
        }

        /* draw on canvas */
        if (wpx_mouse_left_down && (int)wpx_mouse.x < pal_x && (int)wpx_mouse.y >= hud_h)
            wpx_circle_fill((int)wpx_mouse.x, (int)wpx_mouse.y, brush, col);
        if (wpx_mouse_right_down && (int)wpx_mouse.x < pal_x && (int)wpx_mouse.y >= hud_h)
            wpx_circle_fill((int)wpx_mouse.x, (int)wpx_mouse.y, brush, BG);

        /* color picker (middle / wheel click) */
        if (wpx_mouse_wheel_press && (int)wpx_mouse.x < pal_x && (int)wpx_mouse.y >= hud_h) {
            Color32 picked = wpx_getpixel((int)wpx_mouse.x, (int)wpx_mouse.y);
            if (picked != BLANK) col = picked;
        }

        if (wpx_key_press('S')) wpx_screenshot("paint_output.png");
        if (wpx_key_press('C')) wpx_rect_fill(0, hud_h, pal_x, WPX_H - hud_h, BG);

        /* HUD bar (redrawn every frame to stay fresh) */
        wpx_rect_fill(0, 0, WPX_W, hud_h - 1, BLACK);
        wpx_line(0, hud_h - 1, WPX_W, hud_h - 1, DARKGRAY);
        {
            int preview_r = brush > 11 ? 11 : brush;
            wpx_circle_fill(18, hud_h / 2, preview_r, col);
            wpx_circle     (18, hud_h / 2, preview_r, WHITE);
        }
        wpx_text(GRAY, 0, 36, 2,  "brush: %d  |  S=screenshot  C=clear  ESC=quit", brush);
        wpx_text(GRAY, 0, 36, 14, "LEFT=draw  RIGHT=erase  WHEEL=size  MID=pick color");

        /* palette strip */
        wpx_rect_fill(pal_x - 1, 0, WPX_W - pal_x + 1, WPX_H, BLACK);
        wpx_line(pal_x - 1, 0, pal_x - 1, WPX_H, DARKGRAY);
        for (int i = 0; i < npal; i++) {
            int py = hud_h + 8 + i * 26;
            wpx_rect_fill(pal_x + 4, py, 20, 20, palette[i]);
            if (i == sel) wpx_rect(pal_x + 3, py - 1, 22, 22, WHITE);
        }

        winpixel_present();
    }

    return 0;
}
