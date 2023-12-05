#include <string.h>
#include <stdio.h>
#include <dwmapi.h>

#include "window/window.h"
#include "config.h"

#include "panel/panel.h"

#include <time.h>

void ThemeRefresh(HWND hWnd) {
    DWORD lightMode;
    DWORD pcbData = sizeof(lightMode);

    if (RegGetValueW(
            HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
            L"AppsUseLightTheme",
            RRF_RT_REG_DWORD,
            NULL,
            &lightMode,
            &pcbData) == ERROR_SUCCESS)
    {
        BOOL DarkMode = !lightMode;

        /* `20 == DWMWA_USE_IMMERSIVE_DARK_MODE` in Windows 11 SDK.
         * This value was undocumented for Windows 10 versions 2004 and later,
         * supported for Windows 11 Build 22000 and later. */
        DwmSetWindowAttribute(hWnd, 20, &DarkMode, sizeof(DarkMode));
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CTLCOLORSTATIC: {
            HDC hdcStatic = (HDC) wParam;
            SetTextColor(hdcStatic, RGB(255, 255, 255)); 
            SetBkColor(hdcStatic, RGB(0, 0, 255));       // Blue panel background
            return (INT_PTR)CreateSolidBrush(RGB(0, 0, 255));
        }
        // ... otros casos ...
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}



HWND InitWindow(HINSTANCE hInstance, int nCmdShow) {
    WNDCLASSW wc;
    HWND hwnd;

    // Window class registration
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = L"KEngineClass";

    if(!RegisterClassW(&wc)) {
        MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return NULL;
    }

    // Window name creation
    int size = strlen(GAME_ENGINE_NAME) + strlen(GAME_ENGINE_VERSION) + 1 + 3;
    char* windowName = malloc(size);
    if (windowName == NULL) {
        perror("Error allocating memory for window name");
        return NULL;
    }
    snprintf(windowName, size, "%s - %s", GAME_ENGINE_NAME, GAME_ENGINE_VERSION);

    // Main window creation
    hwnd = CreateWindowExW(
        0,
        wc.lpszClassName,
        windowName,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
        NULL, NULL, hInstance, NULL);

    if(hwnd == NULL) {
        MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        free(windowName);
        return NULL;
    }

    // Setting window properties
    HBRUSH brush = CreateSolidBrush(RGB(45, 45, 48));
    SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)brush);
    ThemeRefresh(hwnd);

    // Panel creation and initialization
    Panel mainPanel;
    InitPanel(&mainPanel, hwnd, 50, 50, 300, 200); // Initializes the panel
    SetPanelBackground(&mainPanel, RGB(0, 0, 255)); // Sets the panel background to blue
    AddButtonToPanel(&mainPanel, L"turip", 100, 75, 100, 50); // Adds a button to the panel

    // Displaying the window
    ShowWindow(hwnd, SW_MAXIMIZE);
    UpdateWindow(hwnd);

    // Freeing the window name
    free(windowName);

    return hwnd;
}
