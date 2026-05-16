#include "winpixel.h"

typedef struct {
    LARGE_INTEGER freq;
    LARGE_INTEGER last;
    double        delta;
    double        fps;
    double        acc;
    int           frame_count;
} WPX_FPSTIMER;

static WPX_FPSTIMER _timer_;

void wpx_time_init (void) {

    QueryPerformanceFrequency(&_timer_.freq);
    QueryPerformanceCounter(&_timer_.last);
    _timer_.delta       = 0.0;
    _timer_.fps         = 0.0;
    _timer_.acc         = 0.0;
    _timer_.frame_count = 0;
}

void wpx_time_update (void) {

    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);

    _timer_.delta = (double)(now.QuadPart - _timer_.last.QuadPart)
                  / (double)_timer_.freq.QuadPart;
    _timer_.last  = now;

    _timer_.acc += _timer_.delta;
    _timer_.frame_count++;

    if (_timer_.acc >= 1.0) {
        _timer_.fps         = _timer_.frame_count / _timer_.acc;
        _timer_.frame_count = 0;
        _timer_.acc         = 0.0;
    }
}

WINPIXELDLL double WINPIXELCALL wpx_get_fps (void) {
    return _timer_.fps;
}

WINPIXELDLL double WINPIXELCALL wpx_get_delta (void) {
    return _timer_.delta;
}
