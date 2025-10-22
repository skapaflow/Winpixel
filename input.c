#include <winpixel.h>

int WINPIXELCALL key (int k) {

	return !!KEY[k];
}

int WINPIXELCALL key_press (int k) {

	if (KEYP[k] == 1) {
		KEYP[k] = 2;
		return 1;
	}
	return 0;
}

int WINPIXELCALL key_release (int k) {

	if (KEYR[k] == 1) {
		KEYR[k] = 0;
		return 1;
	}
	return 0;
}