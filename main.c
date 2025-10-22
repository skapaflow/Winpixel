/*
#getline {
	| del main.exe
	| gcc -o main.exe main.c -I. -L. -lwinpixel -s -Wall -mwindows
	| @main.exe
}
*/

#include <winpixel.h> 

int main (void) {

	winpixel_window("winpixel", 800, 600);

	while (wpx_exit) {

		wpx_text_center(WPX_WHITE, 2, wpx_mouse.x, wpx_mouse.y-32, "<Alberto>");

		wpx_line(0, 0, wpx_mouse.x, wpx_mouse.y, WPX_WHITE);

		wpx_circle_fill(wpx_mouse.x, wpx_mouse.y, 10, WPX_RED);
		wpx_circle(wpx_mouse.x, wpx_mouse.y, 20, WPX_GREEN);

		winpixel_render(0x19294E, 2);
	}

	winpixel_close();

	return EXIT_SUCCESS;
}