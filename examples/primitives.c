#include <winpixel.h>

/* Column centers and row centers for the 4x4 grid */
#define CX(c) (100 + (c) * 200)
#define CY(r) (90  + (r) * 118)

static void lbl(int x, int y, const char *s) {
    int tw, th;
    wpx_text_measure(s, 0, &tw, &th);
    wpx_text_ex(0, GRAY, 0, BLACK, 1, x - tw / 2, y, s);
}

int main(void) {
    winpixel_window("Winpixel Primitives", 800, 560, false);

    while (wpx_quit && !wpx_key_down(VK_ESCAPE)) {

        /* ---- Title ---- */
        wpx_text_center(WHITE, 1, 400, 12, "WINPIXEL  PRIMITIVES  GALLERY");

        /* ---- Row separators ---- */
        wpx_line(0, 42,  800, 42,  0x333333FF);
        wpx_line(0, 160, 800, 160, 0x222222FF);
        wpx_line(0, 278, 800, 278, 0x222222FF);
        wpx_line(0, 396, 800, 396, 0x222222FF);
        /* ---- Column separators ---- */
        wpx_line(200, 42, 200, 560, 0x222222FF);
        wpx_line(400, 42, 400, 560, 0x222222FF);
        wpx_line(600, 42, 600, 560, 0x222222FF);

        /* ===== ROW 0: Lines ===== */

        /* wpx_line */
        wpx_line(CX(0)-40, CY(0)-20, CX(0)+40, CY(0)+20, CYAN);
        lbl(CX(0), CY(0)+32, "wpx_line");

        /* wpx_line_gap  (draws middle 50%) */
        wpx_line_gap(CX(1)-40, CY(1-1), CX(1)+40, CY(1-1), CYAN);
        lbl(CX(1), CY(0)+32, "wpx_line_gap");

        /* wpx_line_dashed */
        wpx_line_dashed(CX(2)-45, CY(0), CX(2)+45, CY(0), 12.0f, CYAN);
        lbl(CX(2), CY(0)+32, "wpx_line_dashed");

        /* wpx_spline (thick) */
        wpx_spline(CX(3)-40, CY(0)-20, CX(3)+40, CY(0)+20, 5.0f, CYAN);
        lbl(CX(3), CY(0)+32, "wpx_spline");

        /* ===== ROW 1: Rectangles ===== */

        /* wpx_rect (outline) */
        wpx_rect(CX(0)-35, CY(1)-22, 70, 44, SKYBLUE);
        lbl(CX(0), CY(1)+32, "wpx_rect");

        /* wpx_rect_fill */
        wpx_rect_fill(CX(1)-35, CY(1)-22, 70, 44, SKYBLUE);
        lbl(CX(1), CY(1)+32, "wpx_rect_fill");

        /* wpx_rect_fill_grid */
        wpx_rect_fill_grid(CX(2)-35, CY(2-1)-22, 70, 44, SKYBLUE);
        lbl(CX(2), CY(1)+32, "wpx_rect_fill_grid");

        /* wpx_rect_center */
        wpx_rect_center(CX(3), CY(1), 70, 44, SKYBLUE);
        lbl(CX(3), CY(1)+32, "wpx_rect_center");

        /* ===== ROW 2: Circles ===== */

        /* wpx_circle */
        wpx_circle(CX(0), CY(2), 30, LIME);
        lbl(CX(0), CY(2)+38, "wpx_circle");

        /* wpx_circle_fill */
        wpx_circle_fill(CX(1), CY(2), 30, LIME);
        lbl(CX(1), CY(2)+38, "wpx_circle_fill");

        /* wpx_circle_fill_grid */
        wpx_circle_fill_grid(CX(2), CY(2), 30, LIME);
        lbl(CX(2), CY(2)+38, "wpx_circle_fill_grid");

        /* wpx_ellipse */
        wpx_ellipse(CX(3), CY(2), 44, 26, LIME);
        lbl(CX(3), CY(2)+38, "wpx_ellipse");

        /* ===== ROW 3: Other shapes ===== */

        /* wpx_triangle_fill */
        {
            vec2i tri[3] = {{CX(0), CY(3)-28}, {CX(0)-36, CY(3)+24}, {CX(0)+36, CY(3)+24}};
            wpx_triangle_fill(tri, ORANGE);
        }
        lbl(CX(0), CY(3)+36, "wpx_triangle_fill");

        /* wpx_triangle_fill_grid */
        {
            vec2i tri[3] = {{CX(1), CY(3)-28}, {CX(1)-36, CY(3)+24}, {CX(1)+36, CY(3)+24}};
            wpx_triangle_fill_grid(tri, ORANGE);
        }
        lbl(CX(1), CY(3)+36, "wpx_triangle_fill_grid");

        /* wpx_bezier_thick */
        {
            vec2f s = {CX(2)-48.0f, CY(3)+24.0f};
            vec2f e = {CX(2)+48.0f, CY(3)-24.0f};
            wpx_bezier_thick(s, e, 4.0f, ORANGE);
        }
        lbl(CX(2), CY(3)+36, "wpx_bezier_thick");

        /* wpx_spline_dashed */
        wpx_spline_dashed(CX(3)-45, CY(3)-20, CX(3)+45, CY(3)+20, 4.0f, 18.0f, ORANGE);
        lbl(CX(3), CY(3)+36, "wpx_spline_dashed");

        winpixel_render(0x111111FF, 8);
    }
    return 0;
}
