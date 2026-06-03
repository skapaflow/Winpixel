#ifndef WINPIXEL_INTERNAL_H
#define WINPIXEL_INTERNAL_H

#include <winpixel.h>

struct WINPIXEL {
    HWND      screen_handle;
    int       w, h;
    int       screen_w, screen_h;
    bool      stretch;
    bool      custom_res;
    bool      fullscreen;
    LONG      saved_style;
    LONG      saved_ex_style;
    WINDOWPLACEMENT saved_placement;
    char      win_id[256];
    uint32_t *buffer_screen;
};

extern WINPIXEL wpx_render;

static inline Color32 wpx_color_to_pixel(Color32 color) {
    Color32 r = (color >> 24) & 0xFF;
    Color32 g = (color >> 16) & 0xFF;
    Color32 b = (color >>  8) & 0xFF;
    return (r << 16) | (g << 8) | b;
}

void wpx_input_key_down          (int key);
void wpx_input_key_up            (int key);
void wpx_input_mouse_button_down (int button);
void wpx_input_mouse_button_up   (int button);
void wpx_input_mouse_wheel       (int delta);
void wpx_input_frame_reset       (void);

void wpx_time_init   (void);
void wpx_time_update (void);

#endif // WINPIXEL_INTERNAL_H
