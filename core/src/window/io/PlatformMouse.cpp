#include "window/io/PlatformMouse.h"

#ifdef _WIN32
#include <windows.h>

void SetMousePos(int x, int y) {
    SetCursorPos(x, y);
}
#endif

#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>

void SetMousePos(int x, int y) {
    CGPoint point = CGPointMake(x, y);
    CGWarpMouseCursorPosition(point);
}
#endif

#ifdef __linux__
#include <X11/Xlib.h>

void SetMousePos(int x, int y) {
    Display* display = XOpenDisplay(0);
    Window root = DefaultRootWindow(display);
    XWarpPointer(display, None, root, 0, 0, 0, 0, x, y);
    XCloseDisplay(display);
}
#endif
