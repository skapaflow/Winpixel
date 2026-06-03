#include "winpixel_internal.h"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool wpx_quit    = true;
int  WPX_W       = 0;
int  WPX_H       = 0;
int  WPX_W2      = 0;
int  WPX_H2      = 0;
int  WPX_SW  = 0;
int  WPX_SH  = 0;
int  WPX_SW2 = 0;
int  WPX_SH2 = 0;

WINPIXEL wpx_render;

static void wpx_calc_dest_rect (int *dx, int *dy, int *dw, int *dh) {

	*dx = 0; *dy = 0;
	*dw = wpx_render.screen_w;
	*dh = wpx_render.screen_h;
	if (!wpx_render.stretch) {
		float src_ar = (float)wpx_render.w / wpx_render.h;
		float dst_ar = (float)wpx_render.screen_w / wpx_render.screen_h;
		if (src_ar > dst_ar) {
			*dh = (int)(wpx_render.screen_w / src_ar);
			*dy = (wpx_render.screen_h - *dh) / 2;
		} else if (src_ar < dst_ar) {
			*dw = (int)(wpx_render.screen_h * src_ar);
			*dx = (wpx_render.screen_w - *dw) / 2;
		}
	}
}

static void wpx_blit_to_hdc (HDC hdc) {

	BITMAPINFO bmi = {0};
	bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth       = wpx_render.w;
	bmi.bmiHeader.biHeight      = -wpx_render.h;
	bmi.bmiHeader.biPlanes      = 1;
	bmi.bmiHeader.biBitCount    = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	int dx, dy, dw, dh;
	wpx_calc_dest_rect(&dx, &dy, &dw, &dh);

	if (!wpx_render.stretch) {
		PatBlt(hdc, 0,      0,       wpx_render.screen_w,          dy,                            BLACKNESS);
		PatBlt(hdc, 0,      dy + dh, wpx_render.screen_w,          wpx_render.screen_h - dy - dh, BLACKNESS);
		PatBlt(hdc, 0,      dy,      dx,                            dh,                            BLACKNESS);
		PatBlt(hdc, dx + dw, dy,     wpx_render.screen_w - dx - dw, dh,                           BLACKNESS);
	}

	StretchDIBits(hdc,
		dx, dy, dw, dh,
		0,  0,  wpx_render.w, wpx_render.h,
		wpx_render.buffer_screen, &bmi, DIB_RGB_COLORS, SRCCOPY);
}

static LRESULT CALLBACK winpixel_events (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

	switch (msg) {
		case WM_KEYDOWN:
			wpx_input_key_down(wparam);
			break;
		case WM_KEYUP:
			wpx_input_key_up(wparam);
			break;
		case WM_LBUTTONDOWN:
			wpx_input_mouse_button_down(MK_LBUTTON);
			break;
		case WM_LBUTTONUP:
			wpx_input_mouse_button_up(MK_LBUTTON);
			break;
		case WM_RBUTTONDOWN:
			wpx_input_mouse_button_down(MK_RBUTTON);
			break;
		case WM_RBUTTONUP:
			wpx_input_mouse_button_up(MK_RBUTTON);
			break;
		case WM_MBUTTONDOWN:
			wpx_input_mouse_button_down(MK_MBUTTON);
			break;
		case WM_MBUTTONUP:
			wpx_input_mouse_button_up(MK_MBUTTON);
			break;
		case WM_MOUSEWHEEL:
			wpx_input_mouse_wheel(GET_WHEEL_DELTA_WPARAM(wparam));
			break;
		case WM_MOUSEMOVE: {
			int dx, dy, dw, dh;
			wpx_calc_dest_rect(&dx, &dy, &dw, &dh);
			float sx = (float)(SHORT)LOWORD(lparam);
			float sy = (float)(SHORT)HIWORD(lparam);
			wpx_smouse.x = sx;
			wpx_smouse.y = sy;
			float mx = (sx - dx) * wpx_render.w / dw;
			float my = (sy - dy) * wpx_render.h / dh;
			wpx_mouse.x = mx;
			wpx_mouse.y = my;
			// wpx_input_mouse_move(mx, my);
			break;
		}
		case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			wpx_blit_to_hdc(hdc);
			EndPaint(hwnd, &ps);
			break;
		}
		case WM_SIZE: {
			int new_w = (int)LOWORD(lparam);
			int new_h = (int)HIWORD(lparam);
			if (new_w > 0 && new_h > 0) {
				wpx_render.screen_w = new_w;
				wpx_render.screen_h = new_h;
				WPX_SW  = new_w;
				WPX_SH  = new_h;
				WPX_SW2 = new_w / 2;
				WPX_SH2 = new_h / 2;
				if (!wpx_render.custom_res &&
					(new_w != wpx_render.w || new_h != wpx_render.h)) {
					void *new_buf = calloc(new_w * new_h, sizeof(uint32_t));
					if (!new_buf) break;
					wpx_render.w  = new_w;
					wpx_render.h  = new_h;
					WPX_W  = new_w;
					WPX_H  = new_h;
					WPX_W2 = new_w / 2;
					WPX_H2 = new_h / 2;
					free(wpx_render.buffer_screen);
					wpx_render.buffer_screen = new_buf;
				}
			}
			break;
		}
		case WM_CLOSE:
			wpx_quit = false;
			DestroyWindow(hwnd);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return 0;
}

WINPIXELDLL bool WINPIXELCALL winpixel_poll_events(void) {
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		if (msg.message == WM_QUIT)
			return true;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return false;
}

WINPIXELDLL int WINPIXELCALL winpixel_window (const char *title, int w, int h, bool logo) {

	const char wpx_logo_text[] =
		"__          ___       _____ _          _ \n"
		"\\ \\        / (_)     |  __ (_)        | |\n"
		" \\ \\  /\\  / / _ _ __ | |__) |__  _____| |\n"
		"  \\ \\/  \\/ / | | '_ \\|  ___/ \\ \\/ / _ \\ |\n"
		"   \\  /\\  /  | | | | | |   | |>  <  __/ |\n"
		"    \\/  \\/   |_|_| |_|_|   |_/_/\\_\\___|_|\n"
		"+---------------------------------------+\n"
		"|       Simple Windows Graphic lib.     |\n"
		"+---------------------------------------+\n";

	if (logo)
		printf("\n%s", wpx_logo_text);

	HINSTANCE hInstance = GetModuleHandle(NULL);
	const char g_sz_class_name[] = "winpixel_class";

	WNDCLASS wc = {0};
	wc.lpfnWndProc   = winpixel_events;
	wc.hInstance     = hInstance;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszClassName = g_sz_class_name;

	if (!RegisterClass(&wc)) {
		MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return 1;
	}

	wpx_render.w        = w;
	wpx_render.h        = h;
	wpx_render.screen_w = w;
	wpx_render.screen_h = h;
	WPX_SW  = w;
	WPX_SH  = h;
	WPX_SW2 = w / 2;
	WPX_SH2 = h / 2;
	wpx_render.stretch  = true;
	wpx_render.custom_res = false;
	strncpy(wpx_render.win_id, title, sizeof(wpx_render.win_id) - 1);

	/* Compute total window size so client area = w × h */
	RECT rect = {0, 0, w, h};
	AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_CLIENTEDGE);
	int wx = rect.right - rect.left;
	int wy = rect.bottom - rect.top;

	wpx_render.screen_handle = CreateWindowEx(
		WS_EX_CLIENTEDGE, g_sz_class_name, title,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, wx, wy,
		NULL, NULL, hInstance, NULL
	);

	if (wpx_render.screen_handle == NULL) {
		MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return 1;
	}

	/* Center on screen */
	int sx = (GetSystemMetrics(SM_CXSCREEN) - wx) / 2;
	int sy = (GetSystemMetrics(SM_CYSCREEN) - wy) / 2;
	SetWindowPos(wpx_render.screen_handle, NULL, sx, sy, wx, wy, SWP_NOZORDER | SWP_SHOWWINDOW);

	/* Software framebuffer */
	WPX_W  = w;
	WPX_H  = h;
	WPX_W2 = w / 2;
	WPX_H2 = h / 2;
	wpx_render.buffer_screen = calloc(w * h, sizeof(uint32_t));
	if (!wpx_render.buffer_screen) {
		MessageBox(NULL, "Memory allocation failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return 1;
	}

	ShowWindow(wpx_render.screen_handle, SW_NORMAL);
	UpdateWindow(wpx_render.screen_handle);

	wpx_time_init();

	srand(time(NULL));

	return 0;
}

/* After winpixel_window() */
WINPIXELDLL void WINPIXELCALL winpixel_maximize (void) {

	ShowWindow(wpx_render.screen_handle, SW_MAXIMIZE);
}

WINPIXELDLL HWND WINPIXELCALL winpixel_get_hwnd(void) {
	return wpx_render.screen_handle;
}


WINPIXELDLL uint32_t *WINPIXELCALL wpx_get_buffer(void) {
	return wpx_render.buffer_screen;
}

WINPIXELDLL void *WINPIXELCALL wpx_get_internal (void) {
	return (void *) &wpx_render;
}

WINPIXELDLL void WINPIXELCALL winpixel_present (void) {

	HDC hdc_win = GetDC(wpx_render.screen_handle);
	wpx_blit_to_hdc(hdc_win);
	ReleaseDC(wpx_render.screen_handle, hdc_win);
	wpx_time_update();
	wpx_input_frame_reset();
	winpixel_poll_events();
}

WINPIXELDLL void WINPIXELCALL winpixel_render(Color32 background, int delay) {
	winpixel_present();
	winpixel_clear(background);
	Sleep(delay);
}

static bool cursor_visible = true;

WINPIXELDLL void WINPIXELCALL wpx_mouse_hide(void) {
	if (cursor_visible)  { ShowCursor(FALSE); cursor_visible = false; }
}

WINPIXELDLL void WINPIXELCALL wpx_mouse_show(void) {
	if (!cursor_visible) { ShowCursor(TRUE);  cursor_visible = true;  }
}

WINPIXELDLL void WINPIXELCALL wpx_mouse_pos(int x, int y) {

	int dx, dy, dw, dh;
	wpx_calc_dest_rect(&dx, &dy, &dw, &dh);
	POINT pt = {
		(int)((float)x * dw / wpx_render.w + dx + 0.5f),
		(int)((float)y * dh / wpx_render.h + dy + 0.5f)
	};
	ClientToScreen(wpx_render.screen_handle, &pt);
	SetCursorPos(pt.x, pt.y);
}

WINPIXELDLL void WINPIXELCALL wpx_fullscreen (void) {

	HWND hwnd = wpx_render.screen_handle;
	if (!wpx_render.fullscreen) {
		wpx_render.saved_style    = GetWindowLong(hwnd, GWL_STYLE);
		wpx_render.saved_ex_style = GetWindowLong(hwnd, GWL_EXSTYLE);
		wpx_render.saved_placement.length = sizeof(WINDOWPLACEMENT);
		GetWindowPlacement(hwnd, &wpx_render.saved_placement);

		HMONITOR hmon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
		MONITORINFO mi = {.cbSize = sizeof(MONITORINFO)};
		GetMonitorInfo(hmon, &mi);

		SetWindowLong(hwnd, GWL_STYLE,   wpx_render.saved_style   & ~WS_OVERLAPPEDWINDOW);
		SetWindowLong(hwnd, GWL_EXSTYLE, wpx_render.saved_ex_style & ~WS_EX_CLIENTEDGE);
		SetWindowPos(hwnd, HWND_TOP,
			mi.rcMonitor.left,
			mi.rcMonitor.top,
			mi.rcMonitor.right  - mi.rcMonitor.left,
			mi.rcMonitor.bottom - mi.rcMonitor.top,
			SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
		wpx_render.fullscreen = true;
	} else {
		SetWindowLong(hwnd, GWL_STYLE,   wpx_render.saved_style);
		SetWindowLong(hwnd, GWL_EXSTYLE, wpx_render.saved_ex_style);
		SetWindowPlacement(hwnd, &wpx_render.saved_placement);
		SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
		wpx_render.fullscreen = false;
	}
}

WINPIXELDLL void WINPIXELCALL wpx_screen_resolution(int w, int h, bool stretch) {
	uint32_t *new_buf = calloc(w * h, sizeof(uint32_t));
	if (!new_buf) return;
	free(wpx_render.buffer_screen);
	wpx_render.buffer_screen = new_buf;
	wpx_render.w    = w;
	wpx_render.h    = h;
	WPX_W  = w;
	WPX_H  = h;
	WPX_W2 = w / 2;
	WPX_H2 = h / 2;
	wpx_render.stretch    = stretch;
	wpx_render.custom_res = true;
}
