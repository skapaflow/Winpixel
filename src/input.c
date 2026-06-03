#include "winpixel.h"
#include <string.h>

static int wpx_button_index(int button) {
    switch (button) {
        case MK_LBUTTON: return 0;
        case MK_RBUTTON: return 1;
        case MK_MBUTTON: return 2;
        default:         return -1;
    }
}

bool    wpx_keys[WPX_MAX_KEYS]                   = {false};
bool    wpx_keys_press[WPX_MAX_KEYS]             = {false};
bool    wpx_keys_up[WPX_MAX_KEYS]                = {false};
bool    wpx_mouse_buttons[WPX_MAX_MOUSE_BUTTONS] = {false};
vec2f   wpx_mouse                                = {0.0f, 0.0f};
vec2f   wpx_smouse                               = {0.0f, 0.0f};

int8_t  wpx_mouse_wheel       = 0;
bool    wpx_mouse_wheel_down  = false;
bool    wpx_mouse_wheel_press = false;
bool    wpx_mouse_left_down   = false;
bool    wpx_mouse_left_press  = false;
bool    wpx_mouse_left_up     = false;
bool    wpx_mouse_right_down  = false;
bool    wpx_mouse_right_press = false;
bool    wpx_mouse_right_up    = false;

// -------------------------
// Public API
// -------------------------

WINPIXELDLL bool WINPIXELCALL wpx_key_down(int key) {
    if (key >= 0 && key < WPX_MAX_KEYS)
        return wpx_keys[key];
    return false;
}

WINPIXELDLL bool WINPIXELCALL wpx_key_press(int key) {
    if (key >= 0 && key < WPX_MAX_KEYS)
        return wpx_keys_press[key];
    return false;
}

WINPIXELDLL bool WINPIXELCALL wpx_key_up(int key) {
    if (key >= 0 && key < WPX_MAX_KEYS)
        return wpx_keys_up[key];
    return false;
}

WINPIXELDLL bool WINPIXELCALL wpx_mouse_button_down(int button) {
    int idx = wpx_button_index(button);
    if (idx >= 0)
        return wpx_mouse_buttons[idx];
    return false;
}

// -------------------------
// Internal
// -------------------------

void wpx_input_key_down(int key) {
    if (key >= 0 && key < WPX_MAX_KEYS) {
        if (!wpx_keys[key])
            wpx_keys_press[key] = true;
        wpx_keys[key] = true;
    }
}

void wpx_input_key_up(int key) {
    if (key >= 0 && key < WPX_MAX_KEYS) {
        wpx_keys[key]    = false;
        wpx_keys_up[key] = true;
    }
}

void wpx_input_mouse_button_down(int button) {
    int idx = wpx_button_index(button);
    if (idx >= 0)
        wpx_mouse_buttons[idx] = true;
    if (button == MK_LBUTTON) { wpx_mouse_left_down  = true; wpx_mouse_left_press  = true; }
    if (button == MK_RBUTTON) { wpx_mouse_right_down = true; wpx_mouse_right_press = true; }
    if (button == MK_MBUTTON) { wpx_mouse_wheel_down = true; wpx_mouse_wheel_press = true; }
}

void wpx_input_mouse_button_up(int button) {
    int idx = wpx_button_index(button);
    if (idx >= 0)
        wpx_mouse_buttons[idx] = false;
    if (button == MK_LBUTTON) { wpx_mouse_left_down  = false; wpx_mouse_left_up  = true; }
    if (button == MK_RBUTTON) { wpx_mouse_right_down = false; wpx_mouse_right_up = true; }
    if (button == MK_MBUTTON) { wpx_mouse_wheel_down = false; }
}

void wpx_input_mouse_move(float x, float y) {
    wpx_mouse.x = x;
    wpx_mouse.y = y;
}

void wpx_input_mouse_wheel (int delta) {

    wpx_mouse_wheel = (int8_t)(delta > 0 ? 1 : -1);
}

void wpx_input_frame_reset(void) {
    memset(wpx_keys_press, 0, sizeof(wpx_keys_press));
    memset(wpx_keys_up,    0, sizeof(wpx_keys_up));
    wpx_mouse_wheel       = 0;
    wpx_mouse_wheel_press = false;
    wpx_mouse_left_press  = false;
    wpx_mouse_left_up     = false;
    wpx_mouse_right_press = false;
    wpx_mouse_right_up    = false;
}
