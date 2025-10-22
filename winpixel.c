/*
#getline {
	  gcc -o winpixel.o -c winpixel.c -D WINPIXEL_DLL -Wall -I.
	| gcc -o print.o -c print.c -D WINPIXEL_DLL -Wall -I.
	| gcc -o input.o -c input.c -D WINPIXEL_DLL -Wall -I.
	| gcc -o draw.o -c draw.c -D WINPIXEL_DLL -Wall -I.
	| gcc -o math.o -c math.c -D WINPIXEL_DLL -Wall -I.
	| gcc -o audio.o -c audio.c -D WINPIXEL_DLL -Wall -I.
	| gcc -o sprite.o -c sprite.c -D WINPIXEL_DLL -Wall -I.
	| gcc -o winpixel.dll winpixel.o print.o input.o draw.o math.o audio.o sprite.o
	-s -shared -Wl,--subsystem,windows -lgdi32 -lwinmm
	| del *.o
	| cp winpixel.dll "../Users/Alberto F Jr/Desktop/winpixel.dll"
	| cp winpixel.dll lib/winpixel.dll
}
*/

/*
 * To support this project, remember
 * Dennis Ritchie on October 12, 2011.
 */

#include <winpixel.h>

#define WPX_TIMER_ID 10

typedef struct {
	clock_t last_time;
	double  accumulator;
	int     frame_count;
	double  fps;
} WPX_FPSTIMER;

HCURSOR wpx_hcursor;

WINPIXEL wpx_render;
vec2i wpx_mouse = {0,0};
float wpx_fps   = 0.0f;

int WPX_W, WPX_H;
int WPX_W2, WPX_H2; /* dividido por 2 */

int wpx_exit = 1;

int *KEY  = NULL;
int *KEYP = NULL;
int *KEYR = NULL;

int      wpx_mouse_left_nclicks  = 0;
int      wpx_mouse_right_nclicks = 0;
int32_t  wpx_mouse_whell         = 0;
int      wpx_mouse_whell_down    = 0;
int      wpx_mouse_whell_press   = 0;
uint32_t wpx_mouse_left_down     = 0;
uint32_t wpx_mouse_left_press    = 0;
uint32_t wpx_mouse_left_up       = 0;
uint32_t wpx_mouse_right_down    = 0;
uint32_t wpx_mouse_right_press   = 0;
uint32_t wpx_mouse_right_up      = 0;

int wpx_keydown = 0;
int wpx_keypress = 0;
char wpx_keychar[2] = {0,0};
char *wpx_dropfile = NULL;

int set_fullscreen_blt = 0;

int wpx_click_count = 0;

void winpixel_cursor (const char *);
void winpixel_clear (wpx_COLOR);
static LRESULT winpixel_events (HWND, UINT, WPARAM, LPARAM);
int winpixel_screen_close (WINPIXEL *);
void winpixel_dispatch (void);
void winpixel_mouse (vec2i *);
void winpixel_free_bitmap (void);
int winpixel_bitmap (int, int);
int winpixel_update_bitmap (int, int);
void winpixel_present (void);
/* FPS */
WPX_FPSTIMER _fps_;
void __wpx_fps_init__ (WPX_FPSTIMER *);
float __wpx_fps_update__ (WPX_FPSTIMER *);
float __wpx_fps_get__ (const WPX_FPSTIMER *);

HCURSOR wpx_hcursor;

void winpixel_cursor (const char *cursor) {

	wpx_hcursor = LoadCursor(NULL, cursor);
}

void winpixel_clear (wpx_COLOR color) {

	WINPIXEL w = wpx_render;
	if (w.draw_buffer != NULL)
		for (int i = 0; i < (w.screen_w * w.screen_h); i++)
			w.draw_buffer[i] = color;
}

void wpx_drop_file (HWND hwnd, WPARAM wparam) {

	char *wide_text = calloc(MAX_PATH, sizeof(char));
	char *short_path = calloc(MAX_PATH, sizeof(char));

	WORD files = DragQueryFile((HDROP) wparam, 0xFFFF, NULL, 0);

	for (WORD a = 0; a <= files; a++) {
		DragQueryFile((HDROP) wparam, a, wide_text, MAX_PATH);
		/* get Short Path Name */
		long length = GetShortPathName(wide_text, short_path, MAX_PATH);
		if (length == 0) {
			wprintf(L"\n error_GetShortPathName!\n");
			return;
		}
		wpx_dropfile = strdup(wide_text);
		printf("DROP[%s]\n", wide_text);
	}

	wpx_free(wide_text);
	wpx_free(short_path);

	DragFinish((HDROP) wparam);
}

static LRESULT winpixel_events (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

	WINPIXEL wpx = wpx_render;

	switch (msg) {
		case WM_CREATE:
			DragAcceptFiles(hwnd, TRUE);
			break;
		case WM_SIZE: {
				RECT r;
				GetClientRect(wpx.screen_handle, &r);
				int w = (r.right - r.left + 4);
				int h = (r.bottom - r.top + 4);
				if ((WPX_W+WPX_H) != w+h)
					winpixel_update_bitmap(w, h);
			}
			break;
		case WM_DROPFILES: {
				HDC hdc = GetDC(hwnd);
				wpx_drop_file(hwnd, wparam);
				ReleaseDC(hwnd, hdc);
			}
			break;
		case WM_TIMER:
			KillTimer(hwnd, WPX_TIMER_ID);
			wpx_mouse_left_nclicks = 0;
			wpx_mouse_right_nclicks = 0;
			wpx_click_count = 0;
			// wpx_mouse_whell_down = (GetAsyncKeyState(VK_MBUTTON) & 0x8000);
			break;
		// case WM_MOUSEMOVE:
			// wpx_mouse_whell_down = (GetAsyncKeyState(VK_MBUTTON) & 0x8000);
			// wpx_mouse_whell = 0;
			// break;
		case WM_MOUSEWHEEL:
			wpx_mouse_whell += ((short) HIWORD(wparam) < 0 ? -1 : 1);
			break;
		case WM_LBUTTONDOWN:
			if (wpx_click_count == 0)
				SetTimer(hwnd, WPX_TIMER_ID, GetDoubleClickTime(), NULL);
			wpx_click_count++;
			wpx_mouse_left_nclicks = wpx_click_count;
			/* * */
			wpx_mouse_left_press = 1;
			wpx_mouse_left_down = 1;
			wpx_mouse_left_up = 0;
			break;
		case WM_LBUTTONUP:
			wpx_mouse_left_press = 0;
			wpx_mouse_left_down = 0;
			wpx_mouse_left_up = 1;
			break;
		case WM_RBUTTONDOWN:
			if (wpx_click_count == 0)
				SetTimer(hwnd, WPX_TIMER_ID, GetDoubleClickTime(), NULL);
			wpx_click_count++;
			wpx_mouse_right_nclicks = wpx_click_count;
			/* * */
			wpx_mouse_right_press = 1;
			wpx_mouse_right_down = 1;
			wpx_mouse_right_up = 0;
			break;
		case WM_RBUTTONUP:
			wpx_mouse_right_press = 0;
			wpx_mouse_right_down = 0;
			wpx_mouse_right_up = 1;
			break;
		case WM_CHAR:
			wpx_keychar[0] = (char) wparam;
			break;
		case WM_KEYDOWN:
			wpx_keydown = 1;
			wpx_keypress = 1;
			KEY[wparam & 511] = 1;
			if (KEYP[wparam & 511] == 0)
				KEYP[wparam & 511] = 1;
			break;
		case WM_KEYUP:
			wpx_keydown = 0;
			KEY[wparam & 511] = 0;
			KEYP[wparam & 511] = 0;
			KEYR[wparam & 511] = 1;
			break;
		case WM_SETCURSOR:
			SetCursor(wpx_hcursor);
			break;
		case WM_CLOSE:
			wpx_exit = 0;
			wpx_free(KEY);
			wpx_free(KEYP);
			wpx_free(KEYR);
			DestroyWindow(hwnd);
			DragAcceptFiles(hwnd, FALSE);
			break;
		default:
			return DefWindowProc(hwnd, msg, wparam, lparam);
	}

	return 0;
}

int winpixel_screen_close (WINPIXEL *dvc) {

	if (dvc->screen_dc) {
		if (dvc->screen_ob) { 
			SelectObject(dvc->screen_dc, dvc->screen_ob); 
			dvc->screen_ob = NULL; 
		}
		DeleteDC(dvc->screen_dc);
		dvc->screen_dc = NULL;
	}

	if (dvc->screen_bmp) { 
		DeleteObject(dvc->screen_bmp); 
		dvc->screen_bmp = NULL; 
	}

	if (dvc->screen_handle) { 
		CloseWindow(dvc->screen_handle); 
		dvc->screen_handle = NULL; 
	}

	return 0;
}

void winpixel_dispatch (void) {

	MSG msg;

	while (1) {

		if (!PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
			break;

		if (GetMessage(&msg, NULL, 0, 0))
			TranslateMessage(&msg);

		DispatchMessage(&msg);
	}
}


void winpixel_mouse (vec2i *out) {

	WINPIXEL wpx = wpx_render;
	/* mouse */
	POINT p = {0,0}, s = {0,0};
	GetCursorPos(&p);
	ClientToScreen(wpx.screen_handle, &s);
	p.x -= s.x;
	p.y -= s.y;
	*out = (vec2i) {p.x, p.y};
}

// Função para liberar recursos do bitmap atual
void winpixel_free_bitmap (void) {

	WINPIXEL wpx = wpx_render;

	if (wpx.screen_bmp != NULL) {
		SelectObject(wpx.screen_dc, wpx.screen_ob);
		DeleteObject(wpx.screen_bmp);
		wpx.screen_bmp = NULL;
		wpx.screen_ob = NULL;
		wpx.screen_bf = NULL;
	}

	if (wpx.screen_dc != NULL) {
		DeleteDC(wpx.screen_dc);
		wpx.screen_dc = NULL;
	}
}

/* Função para atualizar o tamanho do bitmap */
int winpixel_update_bitmap (int new_w, int new_h) {

	WPX_W = new_w;
	WPX_H = new_h;
	WPX_W2 = WPX_W/2;
	WPX_H2 = WPX_H/2;

	/* Primeiro, libere recursos do bitmap atual (se houver) */
	winpixel_free_bitmap();

	WINPIXEL *wpx = &wpx_render;

	HDC hdc = GetDC(wpx->screen_handle);
	wpx->screen_dc = CreateCompatibleDC(hdc);
	ReleaseDC(wpx->screen_handle, hdc);

	wpx->bitmap = (BITMAPINFO) {
		{sizeof(BITMAPINFOHEADER), new_w, -new_h, 1, 32, BI_RGB,  (new_w*new_h*4), 0, 0, 0, 0},
		{}
	};

	if (wpx->screen_bmp != NULL)
		DeleteObject(wpx->screen_bmp);

	LPVOID aux;
	wpx->screen_bmp = CreateDIBSection(wpx->screen_dc, &wpx->bitmap, DIB_RGB_COLORS, &aux, 0, 0);
	if (wpx->screen_bmp == NULL)
		return 1;

	wpx->screen_ob = (HBITMAP) SelectObject(wpx->screen_dc, wpx->screen_bmp);
	wpx->screen_bf = (unsigned char *) aux;
	wpx->screen_w = new_w;
	wpx->screen_h = new_h;
	wpx->screen_pitch = (new_w * 4);

	/* ser draw buffer */
	if (wpx->screen_bf != NULL) {
		wpx_memset(wpx->screen_bf, 0, (new_w * new_h * 4));
		wpx_malloc(wpx->draw_buffer, (new_w * new_h));
		wpx->draw_buffer = (void *) wpx->screen_bf;
	}

	return 0;
}

int WINPIXELCALL winpixel_window (const char *title, int w, int h) {

	wpx_calloc(KEY, 512);
	wpx_calloc(KEYP, 512);
	wpx_calloc(KEYR, 512);

	WINPIXEL *wpx = &wpx_render;
	wpx->screen_bmp = NULL;

	/* get windows size */
	WPX_W = w;
	WPX_H = h;
	WPX_W2 = WPX_W/2;
	WPX_H2 = WPX_H/2;
	wpx_exit = 1;

	strcpy(wpx->win_id, title);

	/* Step 1: Registering the Window Class */
	const char g_sz_class_name[] = "myWindowClass";
	WNDCLASS wc;
	wc.style         = 0;
	wc.lpfnWndProc   = winpixel_events;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = GetModuleHandle(NULL);
	wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = g_sz_class_name;

	if (!RegisterClass(&wc)) {
		MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return 1;
	}

	winpixel_screen_close(wpx);

	/* Step 2: Creating the Window */
	wpx->screen_handle = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		g_sz_class_name,
		title,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		w, h,
		NULL,
		NULL,
		wc.hInstance,
		NULL
	);

	if (wpx->screen_handle == NULL) {
		MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return 1;
	}

	/* create bitmap buffer */
	winpixel_update_bitmap(w, h);
	
	/* centered windows */
	RECT rect = {0, 0, w, h};
	AdjustWindowRect(&rect, GetWindowLong(wpx->screen_handle, GWL_STYLE), 0);
	int wx, wy, sx, sy;
	wx = rect.right - rect.left;
	wy = rect.bottom - rect.top;
	sx = (GetSystemMetrics(SM_CXSCREEN) - wx) / 2;
	sy = (GetSystemMetrics(SM_CYSCREEN) - wy) / 2;
	sx = __max(sx, 0);
	sy = __max(sy, 0);
	SetWindowPos(wpx->screen_handle, NULL, sx, sy, wx, wy, (SWP_NOCOPYBITS | SWP_NOZORDER | SWP_SHOWWINDOW));
	SetForegroundWindow(wpx->screen_handle);

	/* show windows */
	ShowWindow(wpx->screen_handle, SW_NORMAL);
	winpixel_dispatch();

	/* get desktop size */
	HDC hdc_2 = GetDC(wpx->screen_handle);
	wpx->desktop_w = GetDeviceCaps(hdc_2, DESKTOPHORZRES);
	wpx->desktop_h = GetDeviceCaps(hdc_2, DESKTOPVERTRES);

	/* set random seed */
	srand(time(NULL));
	rand(); /* descartar primeiro número aleatório */

	/* definir tipo de cursor */
	winpixel_cursor(WPX_ARROW);

	/* init mouse value */
	winpixel_mouse(&wpx_mouse);

	/* set FPS */
	__wpx_fps_init__(&_fps_);

	return 0;
}
/* After winpixel_window() */
void WINPIXELCALL winpixel_maximize (void) {

	WINPIXEL wpx = wpx_render;
	ShowWindow(wpx.screen_handle, SW_MAXIMIZE);
}

void winpixel_present (void) {

	WINPIXEL w = wpx_render;
	HDC hdc = GetDC(w.screen_handle);
	// if (set_fullscreen_blt)
		// StretchBlt(hdc, 0, 0, w.desktop_w, w.desktop_h, w.screen_dc, 0, 0, w.screen_w, w.screen_h, SRCCOPY);
	// else
		BitBlt(hdc, 0, 0, w.screen_w, w.screen_h, w.screen_dc, 0, 0, SRCCOPY);
	ReleaseDC(w.screen_handle, hdc);
	winpixel_dispatch();
}

void WINPIXELCALL winpixel_render (wpx_COLOR background, int delay) {

	// wpx_lmouse_press = 0;
	// wpx_lmouse_up = 0;
	// wpx_rmouse_press = 0;
	// wpx_rmouse_up = 0;
	wpx_keypress = 0;
	wpx_keychar[0] = 0;
	wpx_mouse_left_nclicks = 0;
	wpx_mouse_right_nclicks = 0;

	wpx_mouse_left_press = 0;
	wpx_mouse_right_press = 0;

	wpx_mouse_left_up = 0;
	wpx_mouse_right_up = 0;

	static int whell_press = 0;
	wpx_mouse_whell_down = (GetAsyncKeyState(VK_MBUTTON) & 0x8000);
	whell_press = (wpx_mouse_whell_down ? whell_press+1 : 0);
	wpx_mouse_whell_press = (whell_press == 1);

	__wpx_fps_update__(&_fps_);
	wpx_fps = __wpx_fps_get__(&_fps_);
	winpixel_mouse(&wpx_mouse);
	winpixel_present();
	winpixel_clear(background);
	Sleep(delay);
}

void WINPIXELCALL winpixel_close (void) {

	winpixel_free_bitmap();

	wpx_free(KEY);
	wpx_free(KEYP);
	wpx_free(KEYR);
	wpx_free(wpx_dropfile);
}

// Inicializa o temporizador de FPS
void __wpx_fps_init__ (WPX_FPSTIMER *timer) {

	timer->last_time   = clock();
	timer->accumulator = 0.0;
	timer->frame_count = 0;
	timer->fps         = 0.0;
}

// Atualiza o temporizador de FPS. Retorna delta time (em segundos).
float __wpx_fps_update__ (WPX_FPSTIMER *timer) {

	clock_t current_time = clock();
	float delta_time    = (float)(current_time - timer->last_time) / CLOCKS_PER_SEC;
	timer->last_time     = current_time;

	timer->accumulator += delta_time;
	timer->frame_count++;

	if (timer->accumulator >= 1.0) {
		timer->fps = timer->frame_count / timer->accumulator;
		timer->frame_count = 0;
		timer->accumulator = 0.0;
	}

	return delta_time;
}

// Obtém o valor atual de FPS
float __wpx_fps_get__ (const WPX_FPSTIMER *timer) {

	return timer->fps;
}