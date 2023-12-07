#include <windows.h>

#include "panel/panel.h"
#include "hwndmap/hwndmap.h"

#define MINIMIZE_BUTTON 1001
#define MIN_PANEL_SIZE 30  // Min size of the panel (width or height depending on the side)
#define PANEL_BORDER 10  // Width of the border used for resizing

wchar_t* panelClassName = L"KCustomPanelClass";

void InitPanel(Panel *panel, HWND hwndParent, bool isLeftAligned);
LRESULT CALLBACK PanelProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void RegisterPanelClass(HINSTANCE hInstance) {
    WNDCLASSEXW wc = {0};
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = PanelProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = panelClassName;
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassExW(&wc)) {
        MessageBox(NULL, "Panel Class Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
    }
}

Panel* CreateNewPanel(HWND hwndParent, bool isLeftAligned) {
    Panel* newPanel = (Panel*)malloc(sizeof(Panel));
    if (newPanel == NULL) {
        return NULL;
    }

    InitPanel(newPanel, hwndParent, isLeftAligned);

    AddPanelToList(newPanel->hwnd, newPanel);

    return newPanel;
}

// Initialize the panel with specified dimensions and position
void InitPanel(Panel *panel, HWND hwndParent, bool isLeftAligned) {
    int defaultWidth = 200;

    panel->isLeftAligned = true;
    panel->resizing = false;
    if (isLeftAligned) {
        panel->x = 0;
        panel->y = 0;
        panel->height = GetSystemMetrics(SM_CYSCREEN);
        panel->width = defaultWidth;
    } else {
        panel->x = GetSystemMetrics(SM_CXSCREEN) - defaultWidth;
        panel->y = 0;
        panel->height = GetSystemMetrics(SM_CYSCREEN);
        panel->width = defaultWidth;
    }

    panel->background = CreateSolidBrush(RGB(30, 30, 40));
    panel->hwnd = CreateWindowExW(
        0,
        panelClassName,
        NULL,
        WS_CHILD | WS_VISIBLE,
        panel->x, panel->y, panel->width, panel->height,
        hwndParent,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hwndParent, GWLP_HINSTANCE),
        NULL
    );
}

void RemovePanel(Panel *panel) {
    RemovePanelFromList(panel->hwnd);
    free(panel);
}

// Window procedure for handling panel messages
LRESULT CALLBACK PanelProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // Retrieve the pointer to the Panel structure stored in GWLP_USERDATA
    Panel *panel = FindPanelInList(hwnd);

    if (panel == NULL)
        return DefWindowProc(hwnd, uMsg, wParam, lParam);

    switch (uMsg) {
        // Resizing panel
        case WM_SETCURSOR: {
            POINT pt;
            GetCursorPos(&pt);
            ScreenToClient(hwnd, &pt);

            if (pt.x >= panel->width - PANEL_BORDER && pt.x <= panel->width) {
                SetCursor(LoadCursor(NULL, IDC_SIZEWE));
                return TRUE;
            }
            break;
        }
        case WM_LBUTTONDOWN: {
            POINT pt;
            GetCursorPos(&pt);
            ScreenToClient(hwnd, &pt);

            // Verify that the cursor is in the border
            if (pt.x >= panel->width - PANEL_BORDER && pt.x <= panel->width) {
                SetCapture(hwnd);  // Capture the mouse
                panel->resizing = TRUE;  // Mark that the panel is being resized
            }
            break;
        }
        case WM_MOUSEMOVE: {
            if (panel->resizing && (wParam & MK_LBUTTON)) {
                POINT pt;
                GetCursorPos(&pt);
                ScreenToClient(hwnd, &pt);

                // Set the new width of the panel
                panel->width = max(MIN_PANEL_SIZE, pt.x);
                SetWindowPos(hwnd, NULL, 0, 0, panel->width, panel->height, SWP_NOMOVE | SWP_NOZORDER);
            }
            break;
        }
        case WM_LBUTTONUP: {
            if (panel->resizing) {
                ReleaseCapture();  // Release the mouse
                panel->resizing = FALSE;  // Mark that the panel is not being resized
            }
            break;
        }
        case WM_ERASEBKGND:
            return (LRESULT)1; // Mark that the background has been erased, but don't draw anything
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            FillRect(hdc, &ps.rcPaint, panel->background);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_SIZE: {
            if (panel) {
                // Fix height
                panel->height = GetSystemMetrics(SM_CYSCREEN);
            }
            return 0;
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
