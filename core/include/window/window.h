#ifndef WINDOW_H
#define WINDOW_H

#include <windows.h>

// Window initialization and creation
HWND InitWindow(HINSTANCE hInstance, int nCmdShow);

// Window procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif // WINDOW_H
