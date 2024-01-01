#include <windowsx.h>
#include <windows.h>
#include <stdio.h>

#include "panel/panel.h"
#include "hwndmap/hwndmap.h"

// Constants for UI elements
#define BUTTON_MINIMIZE 1001
#define BUTTON_HOVERING_COLOR RGB(100, 100, 100)
#define BUTTON_MINIMIZE_LINE_COLOR RGB(160, 160, 160)
#define BUTTON_OFFSET_RIGHT 25
#define BUTTON_OFFSET_TOP 5
#define BUTTON_WIDTH 20
#define BUTTON_HEIGHT 15

#define MIN_PANEL_SIZE 30
#define PANEL_BORDER 10
#define PANEL_HEADER_HEIGHT 25

#define PANELS_BORDER_LINE_COLOR RGB(108, 108, 108)

#define PANEL_TITLE_TEXT_OFFSET_X 10
#define PANEL_TITLE_TEXT_OFFSET_Y 4
#define PANEL_TITLE_TEXT_COLOR RGB(230, 230, 230)
#define PANEL_TITLE_TEXT_OFFSET_BUTTON 10

#define PANEL_BACKGROUND_COLOR RGB(34, 34, 34)
#define PANEL_HEADER_HOVERING_COLOR RGB(37, 37, 37)

// Global variables for brushes
HBRUSH brushBackground;
HBRUSH brushHeaderHovering;

wchar_t* panelClassName = L"KCustomPanelClass";

// Function prototypes
void InitPanel(Panel *panel, HWND hwndParent, int sideAssigned, int fixed);
LRESULT CALLBACK PanelProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ButtonProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void RegisterPanelClass(HINSTANCE hInstance);
HWND CreateCustomButton(HWND hwndParent, HINSTANCE hInstance, int x, int y, int width, int height, int id);
void FreeAllPanels();
void RemovePanel(Panel *panel);

// Register the panel class with customized window procedure
void RegisterPanelClass(HINSTANCE hInstance) {
    WNDCLASSEXW wc = {0};
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = PanelProc;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszClassName = panelClassName;
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassExW(&wc)) {
        MessageBox(NULL, "Panel Class Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return;
    }

    brushBackground = CreateSolidBrush(PANEL_BACKGROUND_COLOR);
    brushHeaderHovering = CreateSolidBrush(PANEL_HEADER_HOVERING_COLOR);
}

// Create and initialize a new panel
Panel* CreateNewPanel(char* name, HWND hwndParent, int sideAssigned, int fixed) {
    Panel* newPanel = (Panel*)malloc(sizeof(Panel));
    if (!newPanel) return NULL;

    newPanel->name = strdup(name);
    if (!newPanel->name) {
        free(newPanel);
        return NULL;
    }

    InitPanel(newPanel, hwndParent, sideAssigned, fixed);
    AddPanelToList(newPanel->hwnd, newPanel);

    return newPanel;
}

// Initialize the panel with specified dimensions and position
void InitPanel(Panel *panel, HWND hwndParent, int sideAssigned, int fixed) {
    // TODO: If side assigned matches with another panel, adapt the position and size of both panels
    int defaultWidth = 200;
    panel->sideAssigned = sideAssigned;
    panel->resizing = false;
    panel->hovering = false;
    panel->fixed = fixed;
    panel->hoveringMinimizeButton = false;

    // Positioning based on the assigned side
    panel->x = (sideAssigned == LEFT_PANEL) ? 0 : GetSystemMetrics(SM_CXSCREEN) - defaultWidth;
    panel->y = 0;
    panel->height = GetSystemMetrics(SM_CYSCREEN);
    panel->width = defaultWidth;

    // Create panel window
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

    // Create minimize button
    panel->minimizeButton = CreateCustomButton(
        panel->hwnd,
        (HINSTANCE)GetWindowLongPtr(hwndParent, GWLP_HINSTANCE),
        panel->width - BUTTON_OFFSET_RIGHT, BUTTON_OFFSET_TOP, BUTTON_WIDTH, BUTTON_HEIGHT,
        BUTTON_MINIMIZE
    );
}

// Custom button creation
HWND CreateCustomButton(HWND hwndParent, HINSTANCE hInstance, int x, int y, int width, int height, int id) {
    HWND hwndButton = CreateWindow(
        "BUTTON", "", WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
        x, y, width, height, 
        hwndParent, (HMENU)(INT_PTR) id, hInstance, NULL
    );
    SetWindowLongPtr(hwndButton, GWLP_WNDPROC, (LONG_PTR)ButtonProc);
    return hwndButton;
}

// Cleanup function to free allocated resources
void FreeAllPanels() {
    if (brushBackground) DeleteObject(brushBackground);
    if (brushHeaderHovering) DeleteObject(brushHeaderHovering);
    RemoveAllPanelsFromList();
}

// Remove a panel and free its resources
void RemovePanel(Panel *panel) {
    RemovePanelFromList(panel->hwnd);
    free(panel);
}

void UpdatePanelsPosition(HWND windowHwnd) {
    PanelNode* current = GetPanelList();
    RECT rect;
    GetClientRect(windowHwnd, &rect);

    int screenWidth = rect.right - rect.left;
    int screenHeight = rect.bottom - rect.top;

    while (current) {
        Panel* panel = current->panel;
        if (panel) { // Asegúrate de que el panel no es NULL
            switch(panel->sideAssigned) {
                case LEFT_PANEL:
                    // Los paneles del lado izquierdo pueden permanecer donde están en x=0.
                    panel->x = 0;
                    panel->y = 0;
                    break;
                case RIGHT_PANEL:
                    // Los paneles del lado derecho deben moverse para mantener su borde alineado con el borde derecho de la pantalla.
                    panel->x = screenWidth - panel->width;
                    panel->y = 0;
                    break;
                case TOP_PANEL:
                    // Los paneles superiores pueden permanecer donde están en y=0.
                    panel->x = 0;
                    panel->y = 0;
                    break;
                case BOTTOM_PANEL:
                    // Los paneles inferiores deben moverse para mantener su borde alineado con el borde inferior de la pantalla.
                    panel->x = 0;
                    panel->y = screenHeight - panel->height;
                    break;
            }
            // Mueve y cambia el tamaño del panel.
            MoveWindow(panel->hwnd, panel->x, panel->y, panel->width, panel->height, TRUE);
        }
        current = current->next; // Avanza al siguiente nodo
    }
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

            if (panel->fixed) break;
            switch(panel->sideAssigned) {
                case LEFT_PANEL: {
                    if (pt.x >= panel->width - PANEL_BORDER && pt.x <= panel->width) {
                        SetCursor(LoadCursor(NULL, IDC_SIZEWE));
                        return TRUE;
                    }
                    break;
                }
                case RIGHT_PANEL: {
                    if (pt.x >= 0 && pt.x <= PANEL_BORDER) {
                        SetCursor(LoadCursor(NULL, IDC_SIZEWE));
                        return TRUE;
                    }
                    break;
                }
                case TOP_PANEL: {
                    if (pt.y >= panel->height - PANEL_BORDER && pt.y <= panel->height) {
                        SetCursor(LoadCursor(NULL, IDC_SIZENS));
                        return TRUE;
                    }
                    break;
                }
                case BOTTOM_PANEL: {
                    if (pt.y >= 0 && pt.y <= PANEL_BORDER) {
                        SetCursor(LoadCursor(NULL, IDC_SIZENS));
                        return TRUE;
                    }
                    break;
                }
            }
            break;
        }
        case WM_LBUTTONDOWN: {
            POINT pt;
            GetCursorPos(&pt);
            ScreenToClient(hwnd, &pt);

            // Verify that the cursor is in the border
            if (panel->fixed) break;

            switch (panel->sideAssigned) {
                case LEFT_PANEL: {
                    if (pt.x >= panel->width - PANEL_BORDER && pt.x <= panel->width) {
                        SetCapture(hwnd);  // Capture the mouse
                        panel->resizing = TRUE;  // Mark that the panel is being resized
                    }
                    break;
                }
                case RIGHT_PANEL: {
                    if (pt.x >= 0 && pt.x <= PANEL_BORDER) {
                        SetCapture(hwnd);  // Capture the mouse
                        panel->resizing = TRUE;  // Mark that the panel is being resized
                    }
                    break;
                }
                case TOP_PANEL: {
                    if (pt.y >= panel->height - PANEL_BORDER && pt.y <= panel->height) {
                        SetCapture(hwnd);  // Capture the mouse
                        panel->resizing = TRUE;  // Mark that the panel is being resized
                    }
                    break;
                }
                case BOTTOM_PANEL: {
                    if (pt.y >= 0&& pt.y <= PANEL_BORDER) {
                        SetCapture(hwnd);  // Capture the mouse
                        panel->resizing = TRUE;  // Mark that the panel is being resized
                    }
                    break;
                }
            }
            
            break;
        }
        case WM_MOUSEMOVE: {
            if (panel->resizing && (wParam & MK_LBUTTON)) {
                POINT pt;
                GetCursorPos(&pt);
                ScreenToClient(hwnd, &pt);

                switch(panel->sideAssigned) {
                    case LEFT_PANEL: {
                        // Set the new width of the panel
                        panel->width = max(MIN_PANEL_SIZE, pt.x);
                        MoveWindow(hwnd, panel->x, panel->y, panel->width, panel->height, TRUE);
                        break;
                    }
                    case RIGHT_PANEL: {
                        // Set the new width of the panel
                        panel->width = max(MIN_PANEL_SIZE, panel->width - pt.x);
                        RECT rect;
                        GetClientRect(GetParent(hwnd), &rect);
                        panel->x = rect.right - rect.left - panel->width;
                        MoveWindow(hwnd, panel->x, panel->y, panel->width, panel->height, TRUE);
                        break;
                    }
                    case TOP_PANEL: {
                        // Set the new height of the panel
                        panel->height = max(MIN_PANEL_SIZE, pt.y);
                        MoveWindow(hwnd, panel->x, panel->y, panel->width, panel->height, TRUE);
                        break;
                    }
                    case BOTTOM_PANEL: {
                        // Set the new height of the panel
                        panel->height = max(MIN_PANEL_SIZE, panel->height - pt.y);
                        RECT rect;
                        GetClientRect(GetParent(hwnd), &rect);
                        panel->y = rect.bottom - rect.top - panel->height;
                        MoveWindow(hwnd, panel->x, panel->y, panel->width, panel->height, TRUE);
                        break;
                    }
                }
            } else {
                if (!panel->hovering) {
                    // Cursor is hovering the panel
                    panel->hovering = TRUE;

                    TRACKMOUSEEVENT tme; // Removed when MOUSELEAVE
                    tme.cbSize = sizeof(TRACKMOUSEEVENT);
                    tme.dwFlags = TME_LEAVE;
                    tme.hwndTrack = hwnd;
                    TrackMouseEvent(&tme);

                    RECT rect = {0, 0, panel->width, PANEL_HEADER_HEIGHT};
                    InvalidateRect(hwnd, &rect, FALSE);
                }
            }
            break;
        }
        case WM_MOUSEHOVER: {
            panel->hovering = TRUE;
            // Cursor is hovering any panel component
            RECT rect = {0, 0, panel->width, PANEL_HEADER_HEIGHT};
            InvalidateRect(hwnd, &rect, FALSE);
            break;
        }
        case WM_MOUSELEAVE: {
            // Cursor out of the panel
            panel->hovering = FALSE;
            RECT rect = {0, 0, panel->width, PANEL_HEADER_HEIGHT};
            InvalidateRect(hwnd, &rect, FALSE);
            break;
        }
        case WM_LBUTTONUP: {
            if (panel->resizing) {
                ReleaseCapture();  // Release the mouse
                panel->resizing = FALSE;  // Mark that the panel is not being resized

                RECT rect = {0, 0, panel->width, PANEL_HEADER_HEIGHT};
                InvalidateRect(hwnd, &rect, FALSE);
            }
            break;
        }
        case WM_ERASEBKGND:
            return (LRESULT)1; // Mark that the background has been erased, but don't draw anything
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rect;
            GetClientRect(hwnd, &rect);

            // Double buffering
            HDC memDC = CreateCompatibleDC(hdc);
            HBITMAP memBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
            HBITMAP oldBitmap = SelectObject(memDC, memBitmap);

            // Body
            FillRect(memDC, &rect, brushBackground);

            // Header
            RECT headerRect = {0, 0, rect.right, PANEL_HEADER_HEIGHT};
            HBRUSH headerBrush = (panel->hovering) ? brushHeaderHovering : brushBackground;

            FillRect(memDC, &headerRect, headerBrush);

            // Text of header
            SetTextColor(memDC, PANEL_TITLE_TEXT_COLOR);
            SetBkMode(memDC, TRANSPARENT);
            RECT textRect = {PANEL_TITLE_TEXT_OFFSET_X, PANEL_TITLE_TEXT_OFFSET_Y, rect.right - BUTTON_WIDTH - PANEL_TITLE_TEXT_OFFSET_BUTTON, PANEL_HEADER_HEIGHT};
            DrawText(memDC, panel->name, -1, &textRect, DT_LEFT | DT_END_ELLIPSIS);

            // Borders
            HPEN hPen = CreatePen(PS_SOLID, 2, PANELS_BORDER_LINE_COLOR);
            HPEN hOldPen = SelectObject(memDC, hPen);
            switch(panel->sideAssigned) {
                case LEFT_PANEL:
                    MoveToEx(memDC, panel->width, 0, NULL);
                    LineTo(memDC, panel->width, panel->height);
                    MoveToEx(memDC, 0, panel->height, NULL);
                    LineTo(memDC, panel->width, panel->height);
                    break;
                case TOP_PANEL:
                    MoveToEx(memDC, 0, panel->height - 1, NULL);
                    LineTo(memDC, panel->width, panel->height - 1);
                    break;
                case RIGHT_PANEL:
                    MoveToEx(memDC, 0, 0, NULL);
                    LineTo(memDC, 0, panel->height);
                    MoveToEx(memDC, 0, panel->height - 1, NULL);
                    LineTo(memDC, panel->width, panel->height - 1);
                    break;
                case BOTTOM_PANEL:
                    MoveToEx(memDC, 0, 0, NULL);
                    LineTo(memDC, panel->width, 0);
                    break;
            }
            
            // Copy the contents of the memory DC to the screen
            BitBlt(hdc, 0, 0, rect.right, rect.bottom, memDC, 0, 0, SRCCOPY);

            // Cleaning
            SelectObject(memDC, oldBitmap);
            DeleteObject(memBitmap);
            DeleteDC(memDC);
            SelectObject(hdc, hOldPen);
            DeleteObject(hPen);

            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_SIZE: {
            if (panel) {
                panel->height = GetSystemMetrics(SM_CYSCREEN);
                int newWidth = LOWORD(lParam); // New width of the panel

                RECT oldButtonRect;
                GetClientRect(panel->minimizeButton, &oldButtonRect);
                MapWindowPoints(panel->minimizeButton, hwnd, (LPPOINT)&oldButtonRect, 2);

                int buttonPosX = newWidth - BUTTON_OFFSET_RIGHT;
                MoveWindow(panel->minimizeButton, buttonPosX, BUTTON_OFFSET_TOP, BUTTON_WIDTH, BUTTON_HEIGHT, TRUE);
                InvalidateRect(hwnd, &oldButtonRect, TRUE); // TRUE = Erase background
            }
            return 0;
        }
        case WM_COMMAND: {
            if (LOWORD(wParam) == BUTTON_MINIMIZE) {
                printf("Minimize button pressed\n");
            }
            break;
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK ButtonProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    Panel* panel = FindPanelInList(GetParent(hwnd));

    switch (uMsg) {
        case WM_MOUSEMOVE: {
            if (!panel->hoveringMinimizeButton) {
                panel->hoveringMinimizeButton = TRUE;
            }
            TRACKMOUSEEVENT tme;
            tme.cbSize = sizeof(TRACKMOUSEEVENT);
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hwnd;
            TrackMouseEvent(&tme);
            SendMessage(GetParent(hwnd), WM_MOUSEHOVER, 0, 0);
            break;
        }
        case WM_MOUSELEAVE: {
            TRACKMOUSEEVENT tme;
            tme.cbSize = sizeof(TRACKMOUSEEVENT);
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = GetParent(hwnd);
            TrackMouseEvent(&tme);
            panel->hoveringMinimizeButton = FALSE;
            InvalidateRect(hwnd, NULL, FALSE);
            break;
        }
        case WM_LBUTTONDOWN: {
            printf("minimized\n");
            break;
        }
        case WM_ERASEBKGND:
            return (LRESULT)1; // Mark that the background has been erased, but don't draw anything
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            RECT rect;
            GetClientRect(hwnd, &rect);

            // Double buffering
            HDC memDC = CreateCompatibleDC(hdc);
            HBITMAP memBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
            HBITMAP oldBitmap = SelectObject(memDC, memBitmap);

            SetBkColor(memDC, panel->hovering ? PANEL_HEADER_HOVERING_COLOR : PANEL_BACKGROUND_COLOR);
            HBRUSH hBrush = CreateSolidBrush(GetBkColor(memDC));
            FillRect(memDC, &rect, hBrush);
            DeleteObject(hBrush);

            // Border
            int cornerRadius = 5;

            HBRUSH backgroundBrush;
            if (panel->hoveringMinimizeButton) {
                backgroundBrush = CreateSolidBrush(BUTTON_HOVERING_COLOR);
            } else if (panel->hovering) {
                backgroundBrush = brushHeaderHovering;
            } else {
                backgroundBrush = brushBackground;
            }

            HBRUSH noBorderBrush = backgroundBrush;
            HPEN transparentPen = CreatePen(PS_NULL, 0, RGB(0, 0, 0));

            HGDIOBJ originalBrush = SelectObject(memDC, noBorderBrush);
            HGDIOBJ originalPen = SelectObject(memDC, transparentPen);

            RoundRect(memDC, rect.left, rect.top, rect.right, rect.bottom, cornerRadius, cornerRadius);

            SelectObject(memDC, originalBrush);
            SelectObject(memDC, originalPen);
            DeleteObject(transparentPen);

            // Horizontal line
            HPEN hPen = CreatePen(PS_SOLID, 2, BUTTON_MINIMIZE_LINE_COLOR); 
            HGDIOBJ oldPen = SelectObject(memDC, hPen);
            MoveToEx(memDC, rect.left + 4, rect.bottom / 2 - 1, NULL);
            LineTo(memDC, rect.right - 6, rect.bottom / 2);

            BitBlt(hdc, 0, 0, rect.right, rect.bottom, memDC, 0, 0, SRCCOPY);

            SelectObject(memDC, oldBitmap);
            DeleteObject(memBitmap);
            DeleteDC(memDC);
            SelectObject(hdc, oldPen);
            DeleteObject(hPen);

            EndPaint(hwnd, &ps);
            break;
        }
    }
    return CallWindowProc(DefWindowProc, hwnd, uMsg, wParam, lParam);
}
