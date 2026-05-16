#include <stdio.h>
#include <winpixel.h>

int main (void) {

	winpixel_window ("WinPixel Input Test", 800, 600, true);

	WPX_Sprite *img = wpx_sprite_load("teapot.png");

	while (wpx_quit && !wpx_key_down(VK_ESCAPE)) {

		wpx_sprite_draw_sub(img, wpx_mouse.x, wpx_mouse.y, recti(0,0,img->w,img->h));

		winpixel_render(BLUEPRINT, 1);
	}

	wpx_sprite_free(img);

	return 0;
}