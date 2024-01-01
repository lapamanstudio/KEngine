#include <string.h>
#include <stdio.h>
#include <dwmapi.h>

#include "window/window.h"
#include "panel/panel.h"
#include "hwndmap/hwndmap.h"
#include "config.h"


#include <time.h>

#define BACKGROUND_COLOR RGB(47, 47, 47)

Panel* treeInspector;
Panel* objectInspector;
Panel* projectFiles;

HBRUSH brush_background;

void ThemeRefresh(HWND hWnd);

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            brush_background = CreateSolidBrush(BACKGROUND_COLOR);
            return 0;
        }
        case WM_DESTROY: {
            if (brush_background) {
                DeleteObject(brush_background);
            }
            FreeAllPanels();
            return 0;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            RECT rect;
            GetClientRect(hwnd, &rect);

            PanelNode* current = GetPanelList();
            while (current != NULL) {
                if (current->panel) {
                    ExcludeClipRect(hdc, current->panel->x, current->panel->y, current->panel->x + current->panel->width, current->panel->y + current->panel->height);
                }
                current = current->next;
            }

            FillRect(hdc, &rect, brush_background);

            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_ERASEBKGND:
            return (LRESULT)1;
        case WM_SIZE: {
            UpdatePanelsPosition(hwnd);
            break;
        }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

HWND InitWindow(HINSTANCE hInstance, int nCmdShow) {
    WNDCLASSW wc = {0};
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
    int size = wcslen(GAME_ENGINE_NAME) + wcslen(GAME_ENGINE_VERSION) + 1 + 3; // +1 null operator, +3 " - "
    wchar_t* windowName = (wchar_t*)malloc(size * sizeof(wchar_t));
    if (windowName == NULL) {
        perror("Error allocating memory for window name");
        return NULL;
    }

    // Format the string
    swprintf_s(windowName, size, L"%s - %s", GAME_ENGINE_NAME, GAME_ENGINE_VERSION);

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
    ThemeRefresh(hwnd);

    // Register panel class
    RegisterPanelClass(hInstance);

    // Panel creation and initialization
    treeInspector = CreateNewPanel("Tree View Inspector", hwnd, LEFT_PANEL, false);
    if (treeInspector == NULL) {
        perror("Error creating tree inspector panel");
        return NULL;
    }

    objectInspector = CreateNewPanel("Object Inspector", hwnd, RIGHT_PANEL, false);
    if (objectInspector == NULL) {
        perror("Error creating object inspector panel");
        return NULL;
    }

    projectFiles = CreateNewPanel("Project Files", hwnd, LEFT_PANEL, false);
    if (projectFiles == NULL) {
        perror("Error creating project files panel");
        return NULL;
    }

    // Displaying the window
    ShowWindow(hwnd, SW_MAXIMIZE);
    UpdateWindow(hwnd);

    // Freeing the window name
    free(windowName);

    return hwnd;
}

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
