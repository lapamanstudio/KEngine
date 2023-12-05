// panel.c

#include "panel/panel.h"

// Initialize the panel with specified dimensions and position
void InitPanel(Panel *panel, HWND hwndParent, int x, int y, int width, int height) {
    panel->x = x;
    panel->y = y;
    panel->width = width;
    panel->height = height;
    panel->hwnd = CreateWindowExW(
        0,
        L"STATIC",
        NULL,
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        x, y, width, height,
        hwndParent,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hwndParent, GWLP_HINSTANCE),
        NULL);

    // Store the pointer to the Panel structure in the HWND of the panel window
    SetWindowLongPtr(panel->hwnd, GWLP_USERDATA, (LONG_PTR)panel);
}

// Set the background color of the panel
void SetPanelBackground(Panel *panel, COLORREF color) {
    panel->background = CreateSolidBrush(color);
    SetClassLongPtr(panel->hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)panel->background);
}

// Add a button to the panel at specified position and size
void AddButtonToPanel(Panel *panel, LPCWSTR text, int x, int y, int width, int height) {
    panel->hButton = CreateWindowExW(
        0,
        L"BUTTON",
        text,
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        x, y, width, height,
        panel->hwnd,
        NULL,
        (HINSTANCE)GetWindowLongPtr(panel->hwnd, GWLP_HINSTANCE),
        NULL);
}

// Window procedure for handling panel messages
LRESULT CALLBACK PanelProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // Retrieve the pointer to the Panel structure stored in GWLP_USERDATA
    Panel *panel = (Panel *)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    switch (uMsg) {
        case WM_COMMAND: {
            if ((HWND)lParam == panel->hButton) {
                MessageBox(hwnd, L"Button clicked!", L"Button Event", MB_OK);
                return 0;
            }
            break;
        }
        // ... other cases ...
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
