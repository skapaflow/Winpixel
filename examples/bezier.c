#include <winpixel.h>

static void draw_pt(float x, float y, bool sel, Color32 col) {
    int r = sel ? 12 : 8;
    wpx_circle_fill((int)x, (int)y, r, col);
    wpx_circle     ((int)x, (int)y, r, WHITE);
}

int main(void) {
    winpixel_window("Bezier Curve Editor", 700, 500, false);

    /* cubic control points: start, ctrl1, ctrl2, end */
    vec2f pts[4] = {
        {120.0f, 380.0f},
        {220.0f,  80.0f},
        {480.0f,  80.0f},
        {580.0f, 380.0f},
    };
    bool cubic = true;
    int  drag  = -1;

    Color32 pt_col[4] = {GREEN, YELLOW, ORANGE, RED};

    while (wpx_quit && !wpx_key_down(VK_ESCAPE)) {

        /* pick / drag */
        if (wpx_mouse_left_press) {
            drag = -1;
            int  n       = cubic ? 4 : 3;
            int  idx3[3] = {0, 1, 3};
            for (int i = 0; i < n; i++) {
                int pi = cubic ? i : idx3[i];
                if (distance_point(wpx_mouse.x, wpx_mouse.y, pts[pi].x, pts[pi].y) < 16.0f) {
                    drag = pi;
                    break;
                }
            }
        }
        if (!wpx_mouse_left_down) drag = -1;
        if (drag >= 0) { pts[drag].x = wpx_mouse.x; pts[drag].y = wpx_mouse.y; }

        /* toggle cubic / quadratic */
        if (wpx_mouse_right_press) cubic = !cubic;

        /* guide lines */
        wpx_line_dashed((int)pts[0].x, (int)pts[0].y,
                        (int)pts[1].x, (int)pts[1].y, 16.0f, DARKGRAY);
        if (cubic) {
            wpx_line_dashed((int)pts[2].x, (int)pts[2].y,
                            (int)pts[3].x, (int)pts[3].y, 16.0f, DARKGRAY);
        } else {
            wpx_line_dashed((int)pts[1].x, (int)pts[1].y,
                            (int)pts[3].x, (int)pts[3].y, 16.0f, DARKGRAY);
        }

        /* curve */
        if (cubic) {
            wpx_spline_bezier_cubic(pts, 4, 4.0f, CYAN);
        } else {
            vec2f q[3] = {pts[0], pts[1], pts[3]};
            wpx_spline_bezier_quadratic(q, 3, 4.0f, CYAN);
        }

        /* control points */
        {
            int  n       = cubic ? 4 : 3;
            int  idx3[3] = {0, 1, 3};
            for (int i = 0; i < n; i++) {
                int pi = cubic ? i : idx3[i];
                draw_pt(pts[pi].x, pts[pi].y, drag == pi, pt_col[cubic ? i : (i == 2 ? 3 : i)]);
            }
        }

        /* HUD */
        wpx_text(WHITE, 0, 4, 4,  "MODE: %s", cubic ? "CUBIC (4 pts)" : "QUADRATIC (3 pts)");
        wpx_text(GRAY,  0, 4, 16, "RIGHT CLICK: toggle mode");
        wpx_text(GRAY,  0, 4, WPX_H - 12, "Drag control points with LEFT CLICK  |  ESC=quit");

        winpixel_render(0x1A1A2EFF, 1);
    }
    return 0;
}
