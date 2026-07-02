/* font.c — vector text (.vtf) example with smooth scaling.
 *
 * The font is loaded from a file at runtime (wpx_font_load) and drawn with
 * wpx_text_font / wpx_text_font_center. The coverage cache only rebakes
 * when the scale changes, so zooming (mouse wheel) stays smooth and cheap.
 *
 * Note: the cache keeps up to WPX_FONT_CACHE_SLOTS scales baked at once per
 * font (see src/font.c), so a handful of fixed scales used every frame
 * (like this example's title/lines/centered text below) all stay cached
 * simultaneously — only a scale that's genuinely new needs a rebake.
 */
#include <stdio.h>
#include <winpixel.h>

int main (void) {

	winpixel_window("WinPixel - Vector Font (.vtf)", 800, 600, true);

	WPX_Font *font = wpx_font_load("../fonts/times.vtf");
	if (!font) {
		printf("error: could not load ../fonts/times.vtf\n");
		return 1;
	}

	float scale = 28.0f;   /* pixels per font unit */

	while (wpx_quit && !wpx_key_down(VK_ESCAPE)) {

		/* smooth zoom: only rebakes the cache when the scale changes */
		if (wpx_mouse_wheel > 0) scale *= 1.1f;
		if (wpx_mouse_wheel < 0) scale /= 1.1f;
		if (scale < 4.0f)   scale = 4.0f;
		if (scale > 200.0f) scale = 200.0f;

		/* title, anchored to the top-left corner */
		wpx_text_font(font, WHITE, scale, 20, 20, "Vector font  scale=%.1f", scale);

		/* lots of text: 1 blit per glyph (same scale -> no rebake) */
		for (int i = 0; i < 12; ++i)
			wpx_text_font(font, SKYBLUE, scale, 20, 20 + (int)(scale * 1.2f) * (i + 1),
			              "line %02d  score: %d", i, 1000 + i);

		/* centered, following the mouse */
		wpx_text_font_center(font, YELLOW, scale, (int)wpx_mouse.x, (int)wpx_mouse.y, "center");

		winpixel_render(BLUEPRINT, 1);
	}

	wpx_font_free(font);
	return 0;
}
