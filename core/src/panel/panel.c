#include <windows.h>
#include <windowsx.h>
#include <stdio.h>

#include "panel/panel.h"
#include "hwndmap/hwndmap.h"

// Constants for UI elements
#define BUTTON_MINIMIZE 1001
#define BUTTON_HOVERING_COLOR RGB(100, 100, 100)
#define BUTTON_MINIMIZE_LINE_COLOR RGB(255, 255, 255) // White

#define MIN_PANEL_SIZE 30
#define PANEL_BORDER 10
#define PANEL_HEADER_HEIGHT 25

#define PANEL_TITLE_TEXT_OFFSET_X 10
#define PANEL_TITLE_TEXT_OFFSET_Y 4
#define PANEL_TITLE_TEXT_COLOR RGB(230, 230, 230)

#define PANEL_BACKGROUND_COLOR RGB(34, 34, 34)
#define PANEL_HEADER_HOVERING_COLOR RGB(37, 37, 37)

// Global variables for brushes
HBRUSH brushBackground;
HBRUSH brushHeaderHovering;

wchar_t* panelClassName = L"KCustomPanelClass";

// Function prototypes
void InitPanel(Panel *panel, HWND hwndParent, int sideAssigned);
LRESULT CALLBACK PanelProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ButtonProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void RegisterPanelClass(HINSTANCE hInstance);
Panel* CreateNewPanel(char* name, HWND hwndParent, int sideAssigned);
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
Panel* CreateNewPanel(char* name, HWND hwndParent, int sideAssigned) {
    Panel* newPanel = (Panel*)malloc(sizeof(Panel));
    if (!newPanel) return NULL;

    newPanel->name = strdup(name);
    if (!newPanel->name) {
        free(newPanel);
        return NULL;
    }

    InitPanel(newPanel, hwndParent, sideAssigned);
    AddPanelToList(newPanel->hwnd, newPanel);

    return newPanel;
}

// Initialize the panel with specified dimensions and position
void InitPanel(Panel *panel, HWND hwndParent, int sideAssigned) {
    int defaultWidth = 200;
    panel->sideAssigned = true;
    panel->resizing = false;
    panel->hovering = false;
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
        panel->width - 25, 6, 20, 15,
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
            } else {
                if (!panel->hovering) {
                    // Cursor is hovering the panel
                    panel->hovering = TRUE;

                    TRACKMOUSEEVENT tme; // Removed when MOUSELEAVE
                    tme.cbSize = sizeof(TRACKMOUSEEVENT);
                    tme.dwFlags = TME_LEAVE;
                    tme.hwndTrack = hwnd;
                    TrackMouseEvent(&tme);

                    InvalidateRect(hwnd, NULL, FALSE);
                }
            }
            break;
        }
        case WM_MOUSEHOVER: {
            panel->hovering = TRUE;
            // Cursor is hovering any panel component
            InvalidateRect(hwnd, NULL, FALSE);
            break;
        }
        case WM_MOUSELEAVE: {
            // Cursor out of the panel
            panel->hovering = FALSE;
            InvalidateRect(hwnd, NULL, FALSE);
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

            if (panel->hovering) {
                RECT rect = { 0, 0, panel->width, PANEL_HEADER_HEIGHT };
                GetClientRect(hwnd, &rect);
                FillRect(hdc, &rect, brushHeaderHovering);
            } else {
                FillRect(hdc, &ps.rcPaint, brushBackground);
            }

            // Draw the title
            SetTextColor(hdc, PANEL_TITLE_TEXT_COLOR);
            SetBkMode(hdc, TRANSPARENT);

            int textX = PANEL_TITLE_TEXT_OFFSET_X;
            int textY = PANEL_TITLE_TEXT_OFFSET_Y;

            // Draw the text
            TextOut(hdc, textX, textY, panel->name, strlen(panel->name));

            // Draw body of the panel
            OffsetRect(&ps.rcPaint, 0, PANEL_HEADER_HEIGHT);
            FillRect(hdc, &ps.rcPaint, brushBackground);
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
                InvalidateRect(hwnd, &oldButtonRect, TRUE); // TRUE = Erase background

                int buttonPosX = newWidth - 30; // 20px width + 10px margin
                MoveWindow(panel->minimizeButton, buttonPosX, 6, 20, 15, TRUE);
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
                InvalidateRect(hwnd, NULL, FALSE);
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
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            RECT rect;
            GetClientRect(hwnd, &rect);

            int cornerRadius = 5;

            HBRUSH backgroundBrush;
            if (panel->hoveringMinimizeButton) {
                backgroundBrush = CreateSolidBrush(BUTTON_HOVERING_COLOR);
            } else if (panel->hovering) {
                backgroundBrush = brushHeaderHovering;
            } else {
                backgroundBrush = brushBackground;
            }

            // Crear un pincel sin borde
            HBRUSH noBorderBrush = backgroundBrush;
            HPEN transparentPen = CreatePen(PS_NULL, 0, RGB(0, 0, 0)); // Pluma transparente

            // Guardar el pincel y la pluma originales y seleccionar los nuevos
            HGDIOBJ originalBrush = SelectObject(hdc, noBorderBrush);
            HGDIOBJ originalPen = SelectObject(hdc, transparentPen);

            // Dibujar un rectángulo redondeado sin borde
            RoundRect(hdc, rect.left, rect.top, rect.right, rect.bottom, cornerRadius, cornerRadius);

            // Restaurar el pincel y la pluma originales
            SelectObject(hdc, originalBrush);
            SelectObject(hdc, originalPen);

            // No es necesario eliminar noBorderBrush porque es igual a backgroundBrush
            DeleteObject(transparentPen);

            // Horizontal line
            HPEN hPen = CreatePen(PS_SOLID, 2, RGB(255, 255, 255)); // White line
            HGDIOBJ oldPen = SelectObject(hdc, hPen);
            MoveToEx(hdc, rect.left + 4, rect.bottom / 2, NULL);
            LineTo(hdc, rect.right - 6, rect.bottom / 2);
            SelectObject(hdc, oldPen);
            DeleteObject(hPen);

            EndPaint(hwnd, &ps);
            break;
        }
    }
    return CallWindowProc(DefWindowProc, hwnd, uMsg, wParam, lParam);
}
