#include <winpixel.h>
#include <math.h>

int main (void) {

    winpixel_window("Analog Clock", 400, 400, false);

    int cx = WPX_W2, cy = WPX_H2;
    int r  = 160;

    while (wpx_quit && !wpx_key_down(VK_ESCAPE)) {

        /* get current local time (sub-second via milliseconds) */
        SYSTEMTIME t;
        GetLocalTime(&t);

        float s_angle = (t.wSecond  + t.wMilliseconds / 1000.0f) *  6.0f;
        float m_angle = (t.wMinute  + t.wSecond        /  60.0f) *  6.0f;
        float h_angle = ((t.wHour % 12) + t.wMinute    /  60.0f) * 30.0f;

        /* face */
        wpx_circle_fill(cx, cy, r,     0x1C1C2EFF);
        wpx_circle     (cx, cy, r,     0x4444AAFF);
        wpx_circle     (cx, cy, r - 6, 0x2A2A4AFF);

        /* tick marks */
        for (int i = 0; i < 60; i++) {
            float a   = radsf(i * 6.0f - 90.0f);
            float ca  = cosf(a), sa = sinf(a);
            int   len = (i % 5 == 0) ? 14 : 5;
            int   thk = (i % 5 == 0) ?  2 : 1;
            int   x0  = cx + (int)(ca * (r - len - 4));
            int   y0  = cy + (int)(sa * (r - len - 4));
            int   x1  = cx + (int)(ca * (r - 6));
            int   y1  = cy + (int)(sa * (r - 6));
            if (thk > 1)
                wpx_spline((float)x0, (float)y0, (float)x1, (float)y1, 2.0f,
                           (i % 5 == 0) ? WHITE : GRAY);
            else
                wpx_line(x0, y0, x1, y1, GRAY);
        }

        /* hour hand */
        {
            float a = radsf(h_angle - 90.0f);
            float len = r * 0.50f;
            wpx_spline((float)cx, (float)cy,
                       cx + cosf(a) * len, cy + sinf(a) * len,
                       7.0f, WHITE);
        }

        /* minute hand */
        {
            float a = radsf(m_angle - 90.0f);
            float len = r * 0.74f;
            wpx_spline((float)cx, (float)cy,
                       cx + cosf(a) * len, cy + sinf(a) * len,
                       4.0f, SKYBLUE);
        }

        /* second hand (thin line + tail) */
        {
            float a    = radsf(s_angle - 90.0f);
            float len  = r * 0.84f;
            float tail = r * 0.20f;
            wpx_line(cx - (int)(cosf(a) * tail), cy - (int)(sinf(a) * tail),
                     cx + (int)(cosf(a) * len),  cy + (int)(sinf(a) * len),
                     RED);
        }

        /* center cap */
        wpx_circle_fill(cx, cy, 7, RED);
        wpx_circle     (cx, cy, 7, WHITE);

        /* FPS */
        wpx_text(0x333366FF, 0, 4, 4, "%.0f fps", wpx_get_fps());

        winpixel_render(0x0D0D1AFF, 0);
    }
    return 0;
}
