#include <stdio.h>
#include <winpixel.h>

int main (void) {

	winpixel_window ("Template", 800, 600, true);

	while (wpx_quit && !wpx_key_down(VK_ESCAPE)) {

		wpx_text_center(WHITE, 5, WPX_W2, WPX_H2, "HELLO WORLD!");

		winpixel_render(BLUEPRINT, 1);
	}

	return 0;
}