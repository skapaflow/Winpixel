#ifndef WINPIXEL_INTERNAL_H
#define WINPIXEL_INTERNAL_H

#include <winpixel.h>

struct WINPIXEL {
    HWND      screen_handle;
    int       w, h;
    char      win_id[256];
    uint32_t *buffer_screen;
};

extern WINPIXEL wpx_render;

void wpx_input_key_down          (int key);
void wpx_input_key_up            (int key);
void wpx_input_mouse_button_down (int button);
void wpx_input_mouse_button_up   (int button);
void wpx_input_mouse_move        (float x, float y);
void wpx_input_mouse_wheel       (int delta);
void wpx_input_frame_reset       (void);

void wpx_time_init   (void);
void wpx_time_update (void);

#endif // WINPIXEL_INTERNAL_H
